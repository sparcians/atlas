#include "core/inst_handlers/zicsr/RvzicsrInsts.hpp"
#include "include/ActionTags.hpp"
#include "core/ActionGroup.hpp"

namespace atlas
{
    template <typename XLEN>
    void RvzicsrInsts::getInstHandlers(std::map<std::string, Action> & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same<XLEN, RV32>::value);
        if constexpr (std::is_same_v<XLEN, RV64>)
        {
            inst_handlers.emplace(
                "csrrc", atlas::Action::createAction<&RvzicsrInsts::csrrc_64_handler, RvzicsrInsts>(
                             nullptr, "csrrc", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "csrrci",
                atlas::Action::createAction<&RvzicsrInsts::csrrci_64_handler, RvzicsrInsts>(
                    nullptr, "csrrci", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "csrrs", atlas::Action::createAction<&RvzicsrInsts::csrrs_64_handler, RvzicsrInsts>(
                             nullptr, "csrrs", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "csrrsi",
                atlas::Action::createAction<&RvzicsrInsts::csrrsi_64_handler, RvzicsrInsts>(
                    nullptr, "csrrsi", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "csrrw", atlas::Action::createAction<&RvzicsrInsts::csrrw_64_handler, RvzicsrInsts>(
                             nullptr, "csrrw", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "csrrwi",
                atlas::Action::createAction<&RvzicsrInsts::csrrwi_64_handler, RvzicsrInsts>(
                    nullptr, "csrrwi", ActionTags::EXECUTE_TAG));
        }
        else if constexpr (std::is_same_v<XLEN, RV32>)
        {
            sparta_assert(false, "RV32 is not supported yet!");
        }
    }

    template void RvzicsrInsts::getInstHandlers<RV32>(std::map<std::string, Action> &);
    template void RvzicsrInsts::getInstHandlers<RV64>(std::map<std::string, Action> &);

    ActionGroup* RvzicsrInsts::csrrs_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // bool write = insn.rs1() != 0;
        // int csr = validate_csr(insn.csr(), write);
        // reg_t old = p->get_csr(csr, insn, write);
        // if (write) {
        //     p->put_csr(csr, old | RS1);
        // }
        // WRITE_RD(sext_xlen(old));
        // serialize();

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvzicsrInsts::csrrc_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // bool write = insn.rs1() != 0;
        // int csr = validate_csr(insn.csr(), write);
        // reg_t old = p->get_csr(csr, insn, write);
        // if (write) {
        //     p->put_csr(csr, old & ~RS1);
        // }
        // WRITE_RD(sext_xlen(old));
        // serialize();

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvzicsrInsts::csrrwi_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // int csr = validate_csr(insn.csr(), true);
        // reg_t old = p->get_csr(csr, insn, true);
        // p->put_csr(csr, insn.rs1());
        // WRITE_RD(sext_xlen(old));
        // serialize();

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvzicsrInsts::csrrw_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // int csr = validate_csr(insn.csr(), true);
        // reg_t old = p->get_csr(csr, insn, true);
        // p->put_csr(csr, RS1);
        // WRITE_RD(sext_xlen(old));
        // serialize();

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvzicsrInsts::csrrsi_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // bool write = insn.rs1() != 0;
        // int csr = validate_csr(insn.csr(), write);
        // reg_t old = p->get_csr(csr, insn, write);
        // if (write) {
        //     p->put_csr(csr, old | insn.rs1());
        // }
        // WRITE_RD(sext_xlen(old));
        // serialize();

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvzicsrInsts::csrrci_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // bool write = insn.rs1() != 0;
        // int csr = validate_csr(insn.csr(), write);
        // reg_t old = p->get_csr(csr, insn, write);
        // if (write) {
        //     p->put_csr(csr, old & ~(reg_t)insn.rs1());
        // }
        // WRITE_RD(sext_xlen(old));
        // serialize();

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

} // namespace atlas
