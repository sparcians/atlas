#include "core/inst_handlers/inst_helpers.hpp"
#include "core/inst_handlers/zicsr/RvzicsrInsts.hpp"
#include "include/ActionTags.hpp"
#include "core/ActionGroup.hpp"
#include "core/AtlasState.hpp"
#include "core/AtlasInst.hpp"
#include "core/Exception.hpp"
#include "core/Trap.hpp"

extern "C"
{
#include "source/include/softfloat.h"
}

namespace atlas
{
    template <typename XLEN>
    void RvzicsrInsts::getInstHandlers(Execute::InstHandlersMap & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);
        inst_handlers.emplace(
            "csrrc",
            atlas::Action::createAction<&RvzicsrInsts::csrrc_handler<XLEN>, RvzicsrInsts>(
                nullptr, "csrrc", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "csrrci",
            atlas::Action::createAction<&RvzicsrInsts::csrrci_handler<XLEN>, RvzicsrInsts>(
                nullptr, "csrrci", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "csrrs",
            atlas::Action::createAction<&RvzicsrInsts::csrrs_handler<XLEN>, RvzicsrInsts>(
                nullptr, "csrrs", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "csrrsi",
            atlas::Action::createAction<&RvzicsrInsts::csrrsi_handler<XLEN>, RvzicsrInsts>(
                nullptr, "csrrsi", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "csrrw",
            atlas::Action::createAction<&RvzicsrInsts::csrrw_handler<XLEN>, RvzicsrInsts>(
                nullptr, "csrrw", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "csrrwi",
            atlas::Action::createAction<&RvzicsrInsts::csrrwi_handler<XLEN>, RvzicsrInsts>(
                nullptr, "csrrwi", ActionTags::EXECUTE_TAG));
    }

    template void RvzicsrInsts::getInstHandlers<RV32>(Execute::InstHandlersMap &);
    template void RvzicsrInsts::getInstHandlers<RV64>(Execute::InstHandlersMap &);

    template <typename XLEN>
    void RvzicsrInsts::getCsrUpdateActions(Execute::CsrUpdateActionsMap & csr_update_actions)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);

        csr_update_actions.emplace(
            FCSR,
            atlas::Action::createAction<&RvzicsrInsts::fcsr_update_handler<XLEN>, RvzicsrInsts>(
                nullptr, "fcsr_update"));
        csr_update_actions.emplace(
            FFLAGS,
            atlas::Action::createAction<&RvzicsrInsts::fflags_update_handler<XLEN>, RvzicsrInsts>(
                nullptr, "fflags_update"));
        csr_update_actions.emplace(
            FRM, atlas::Action::createAction<&RvzicsrInsts::frm_update_handler<XLEN>, RvzicsrInsts>(
                nullptr, "frm_update"));
        csr_update_actions.emplace(
            MISA,
            atlas::Action::createAction<&RvzicsrInsts::misa_update_handler<XLEN>, RvzicsrInsts>(
                nullptr, "misa_update"));
        csr_update_actions.emplace(
            MSTATUS,
            atlas::Action::createAction<&RvzicsrInsts::mstatus_update_handler<XLEN>, RvzicsrInsts>(
                nullptr, "mstatus_update"));
        csr_update_actions.emplace(
            SSTATUS,
            atlas::Action::createAction<&RvzicsrInsts::sstatus_update_handler<XLEN>, RvzicsrInsts>(
                nullptr, "sstatus_update"));
    }

    template void RvzicsrInsts::getCsrUpdateActions<RV32>(Execute::CsrUpdateActionsMap &);
    template void RvzicsrInsts::getCsrUpdateActions<RV64>(Execute::CsrUpdateActionsMap &);

    template<bool WRITE>
    bool RvzicsrInsts::isAccessLegal_(const uint32_t csr_num, const PrivMode priv_mode)
    {
        // From RISC-V spec:
        // The upper 4 bits of the CSR address (csr[11:8]) are used to encode the read and write
        // accessibility of the CSRs according to privilege level. The top two bits (csr[11:10])
        // indicate whether the register is read/write (00,01, or 10) or read-only (11). The next
        // two bits (csr[9:8]) encode the lowest privilege level that can access the CSR.
        const bool is_writable = (csr_num & 0xc00) != 0xc00;
        const PrivMode lowest_priv_level = (PrivMode)((csr_num & 0x300) >> 8);

        return (!WRITE || (WRITE && is_writable)) && (priv_mode >= lowest_priv_level);
    }

    template <typename XLEN> ActionGroup* RvzicsrInsts::csrrc_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        const auto rs1 = insn->getRs1();
        auto rd = insn->getRd();
        const uint32_t csr = insn->getCsr();

        if (!isAccessLegal_(csr, state->getPrivMode()))
        {
            THROW_ILLEGAL_INST;
        }

        const XLEN csr_val = READ_CSR_REG<XLEN>(state, csr);
        // Don't write CSR is rs1=x0
        if (rs1 != 0)
        {
            if (!isAccessLegal_<true>(csr, state->getPrivMode()))
            {
                THROW_ILLEGAL_INST;
            }
            // rs1 value is treated as a bit mask to clear bits
            const XLEN rs1_val = READ_INT_REG<XLEN>(state, rs1);
            WRITE_CSR_REG<XLEN>(state, csr, (~rs1_val & csr_val));
        }

        WRITE_INT_REG<XLEN>(state, rd, csr_val);

        return nullptr;
    }

    template <typename XLEN> ActionGroup* RvzicsrInsts::csrrci_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        const XLEN imm = insn->getImmediate();
        const auto rd = insn->getRd();
        const int csr = insn->getCsr();

        if (!isAccessLegal_(csr, state->getPrivMode()))
        {
            THROW_ILLEGAL_INST;
        }

        const XLEN csr_val = READ_CSR_REG<XLEN>(state, csr);
        if (imm)
        {
            if (!isAccessLegal_<true>(csr, state->getPrivMode()))
            {
                THROW_ILLEGAL_INST;
            }
            WRITE_CSR_REG<XLEN>(state, csr, (~imm & csr_val));
        }

        WRITE_INT_REG<XLEN>(state, rd, csr_val);

        return nullptr;
    }

    template <typename XLEN> ActionGroup* RvzicsrInsts::csrrs_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        const auto rs1 = insn->getRs1();
        const auto rd = insn->getRd();
        const int csr = insn->getCsr();

        if (!isAccessLegal_(csr, state->getPrivMode()))
        {
            THROW_ILLEGAL_INST;
        }

        const XLEN csr_val = READ_CSR_REG<XLEN>(state, csr);
        if (rs1 != 0)
        {
            if (!isAccessLegal_<true>(csr, state->getPrivMode()))
            {
                THROW_ILLEGAL_INST;
            }
            // rs1 value is treated as a bit mask to set bits
            const XLEN rs1_val = READ_INT_REG<XLEN>(state, rs1);
            WRITE_CSR_REG<XLEN>(state, csr, (rs1_val | csr_val));
        }

        WRITE_INT_REG<XLEN>(state, rd, csr_val);

        return nullptr;
    }

    template <typename XLEN> ActionGroup* RvzicsrInsts::csrrsi_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        const XLEN imm = insn->getImmediate();
        const auto rd = insn->getRd();
        const int csr = insn->getCsr();

        if (!isAccessLegal_(csr, state->getPrivMode()))
        {
            THROW_ILLEGAL_INST;
        }

        const XLEN csr_val = READ_CSR_REG<XLEN>(state, csr);
        if (imm)
        {
            if (!isAccessLegal_<true>(csr, state->getPrivMode()))
            {
                THROW_ILLEGAL_INST;
            }
            // imm value is treated as a bit mask
            WRITE_CSR_REG<XLEN>(state, csr, (imm | csr_val));
        }

        WRITE_INT_REG<XLEN>(state, rd, csr_val);

        return nullptr;
    }

    template <typename XLEN> ActionGroup* RvzicsrInsts::csrrw_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        const auto rs1 = insn->getRs1();
        const auto rd = insn->getRd();
        const int csr = insn->getCsr();

        const XLEN rs1_val = READ_INT_REG<XLEN>(state, rs1);

        if (!isAccessLegal_<true>(csr, state->getPrivMode()))
        {
            THROW_ILLEGAL_INST;
        }

        // Only read CSR if rd!=x0
        if (rd != 0)
        {
            if (!isAccessLegal_(csr, state->getPrivMode()))
            {
                THROW_ILLEGAL_INST;
            }
            const XLEN csr_val = zext(READ_CSR_REG<XLEN>(state, csr), state->getXlen());
            WRITE_INT_REG<XLEN>(state, rd, csr_val);
        }

        WRITE_CSR_REG<XLEN>(state, csr, rs1_val);

        return nullptr;
    }

    template <typename XLEN> ActionGroup* RvzicsrInsts::csrrwi_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        const XLEN imm = insn->getImmediate();
        const auto rd = insn->getRd();
        const int csr = insn->getCsr();

        if (!isAccessLegal_<true>(csr, state->getPrivMode()))
        {
            THROW_ILLEGAL_INST;
        }

        // Only read CSR if rd!=x0
        if (rd != 0)
        {
            if (!isAccessLegal_(csr, state->getPrivMode()))
            {
                THROW_ILLEGAL_INST;
            }
            const XLEN csr_val = zext(READ_CSR_REG<XLEN>(state, csr), state->getXlen());
            WRITE_INT_REG<XLEN>(state, rd, csr_val);
        }

        WRITE_CSR_REG<XLEN>(state, csr, imm);

        return nullptr;
    }

    template <typename XLEN>
    ActionGroup* RvzicsrInsts::misa_update_handler(atlas::AtlasState* state)
    {
        const XLEN misa_val = READ_CSR_REG<XLEN>(state, MISA);
        auto & ext_manager = state->getExtensionManager();

        // FIXME: Extension manager should maintain inclusions
        auto & inclusions = state->getMavisInclusions();
        for (char ext = 'a'; ext <= 'z'; ++ext)
        {
            if ((misa_val & (1 << ext)) && ext_manager.isEnabled(ext))
            {
                inclusions.emplace(std::string(1, ext));
            }
            else
            {
                inclusions.erase(std::string(1, ext));
            }
        }

        state->updateMavisContext();

        return nullptr;
    }

    template <typename XLEN>
    ActionGroup* RvzicsrInsts::mstatus_update_handler(atlas::AtlasState* state)
    {
        // Non-shared fields of SSTATUS are read-only so writing the MSTATUS value to SSTATUS will
        // only write to the shared fields
        const XLEN mstatus_val = READ_CSR_REG<XLEN>(state, MSTATUS);
        WRITE_CSR_REG<XLEN>(state, SSTATUS, mstatus_val);

        // If FS is set to 0 (off), all floating point extensions are disabled
        const uint32_t fs_val = READ_CSR_FIELD<XLEN>(state, MSTATUS, "fs");
        auto & inclusions = state->getMavisInclusions();
        if (fs_val == 0)
        {
            inclusions.erase("f");
            inclusions.erase("d");
        }
        else
        {
            const bool f_ext_enabled = READ_CSR_FIELD<XLEN>(state, MISA, "f") == 0x1;
            const bool d_ext_enabled = READ_CSR_FIELD<XLEN>(state, MISA, "d") == 0x1;
            if (f_ext_enabled)
            {
                inclusions.emplace("f");
            }
            if (d_ext_enabled)
            {
                inclusions.emplace("d");
            }
        }

        state->updateMavisContext();

        return nullptr;
    }

    template <typename XLEN>
    ActionGroup* RvzicsrInsts::sstatus_update_handler(atlas::AtlasState* state)
    {
        // Update shared fields only
        const XLEN sie_val = READ_CSR_FIELD<XLEN>(state, SSTATUS, "sie");
        WRITE_CSR_FIELD<XLEN>(state, MSTATUS, "sie", sie_val);

        const XLEN spie_val = READ_CSR_FIELD<XLEN>(state, SSTATUS, "spie");
        WRITE_CSR_FIELD<XLEN>(state, MSTATUS, "spie", spie_val);

        const XLEN ube_val = READ_CSR_FIELD<XLEN>(state, SSTATUS, "ube");
        WRITE_CSR_FIELD<XLEN>(state, MSTATUS, "ube", ube_val);

        const XLEN spp_val = READ_CSR_FIELD<XLEN>(state, SSTATUS, "spp");
        WRITE_CSR_FIELD<XLEN>(state, MSTATUS, "spp", spp_val);

        const XLEN vs_val = READ_CSR_FIELD<XLEN>(state, SSTATUS, "vs");
        WRITE_CSR_FIELD<XLEN>(state, MSTATUS, "vs", vs_val);

        const XLEN fs_val = READ_CSR_FIELD<XLEN>(state, SSTATUS, "fs");
        WRITE_CSR_FIELD<XLEN>(state, MSTATUS, "fs", fs_val);

        const XLEN xs_val = READ_CSR_FIELD<XLEN>(state, SSTATUS, "xs");
        WRITE_CSR_FIELD<XLEN>(state, MSTATUS, "xs", xs_val);

        const XLEN sum_val = READ_CSR_FIELD<XLEN>(state, SSTATUS, "sum");
        WRITE_CSR_FIELD<XLEN>(state, MSTATUS, "sum", sum_val);

        const XLEN mxr_val = READ_CSR_FIELD<XLEN>(state, SSTATUS, "mxr");
        WRITE_CSR_FIELD<XLEN>(state, MSTATUS, "mxr", mxr_val);

        if constexpr (std::is_same_v<XLEN, RV64>)
        {
            const XLEN uxl_val = READ_CSR_FIELD<XLEN>(state, SSTATUS, "uxl");
            WRITE_CSR_FIELD<XLEN>(state, MSTATUS, "uxl", uxl_val);
        }

        const XLEN sd_val = READ_CSR_FIELD<XLEN>(state, SSTATUS, "sd");
        WRITE_CSR_FIELD<XLEN>(state, MSTATUS, "sd", sd_val);

        return nullptr;
    }

    template <typename XLEN> void set_softfloat_excpetionFlags(atlas::AtlasState* state)
    {
        XLEN softfloat_exceptionFlags_mask = softfloat_flag_inexact | softfloat_flag_underflow
                                             | softfloat_flag_overflow | softfloat_flag_infinite
                                             | softfloat_flag_invalid;
        softfloat_exceptionFlags =
            READ_CSR_REG<XLEN>(state, FFLAGS) & softfloat_exceptionFlags_mask;
    }

    template <typename XLEN>
    ActionGroup* RvzicsrInsts::fcsr_update_handler(atlas::AtlasState* state)
    {
        // FFLAGS
        const XLEN nx_val = READ_CSR_FIELD<XLEN>(state, FCSR, "NX");
        WRITE_CSR_FIELD<XLEN>(state, FFLAGS, "NX", nx_val);

        const XLEN uf_val = READ_CSR_FIELD<XLEN>(state, FCSR, "UF");
        WRITE_CSR_FIELD<XLEN>(state, FFLAGS, "UF", uf_val);

        const XLEN of_val = READ_CSR_FIELD<XLEN>(state, FCSR, "OF");
        WRITE_CSR_FIELD<XLEN>(state, FFLAGS, "OF", of_val);

        const XLEN dz_val = READ_CSR_FIELD<XLEN>(state, FCSR, "DZ");
        WRITE_CSR_FIELD<XLEN>(state, FFLAGS, "DZ", dz_val);

        const XLEN nv_val = READ_CSR_FIELD<XLEN>(state, FCSR, "NV");
        WRITE_CSR_FIELD<XLEN>(state, FFLAGS, "NV", nv_val);

        // FRM
        const XLEN frm_val = READ_CSR_FIELD<XLEN>(state, FCSR, "frm");
        WRITE_CSR_REG<XLEN>(state, FRM, frm_val);

        set_softfloat_excpetionFlags<XLEN>(state);

        return nullptr;
    }

    template <typename XLEN> ActionGroup* RvzicsrInsts::fflags_update_handler(AtlasState* state)
    {
        // FCSR
        const XLEN nx_val = READ_CSR_FIELD<XLEN>(state, FFLAGS, "NX");
        WRITE_CSR_FIELD<XLEN>(state, FCSR, "NX", nx_val);

        const XLEN uf_val = READ_CSR_FIELD<XLEN>(state, FFLAGS, "UF");
        WRITE_CSR_FIELD<XLEN>(state, FCSR, "UF", uf_val);

        const XLEN of_val = READ_CSR_FIELD<XLEN>(state, FFLAGS, "OF");
        WRITE_CSR_FIELD<XLEN>(state, FCSR, "OF", of_val);

        const XLEN dz_val = READ_CSR_FIELD<XLEN>(state, FFLAGS, "DZ");
        WRITE_CSR_FIELD<XLEN>(state, FCSR, "DZ", dz_val);

        const XLEN nv_val = READ_CSR_FIELD<XLEN>(state, FFLAGS, "NV");
        WRITE_CSR_FIELD<XLEN>(state, FCSR, "NV", nv_val);

        set_softfloat_excpetionFlags<XLEN>(state);

        return nullptr;
    }

    template <typename XLEN> ActionGroup* RvzicsrInsts::frm_update_handler(AtlasState* state)
    {
        // FCSR
        const XLEN frm_val = READ_CSR_REG<XLEN>(state, FRM);
        WRITE_CSR_FIELD<XLEN>(state, FCSR, "frm", frm_val);

        return nullptr;
    }
} // namespace atlas
