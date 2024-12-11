#include "core/inst_handlers/i/RviInsts.hpp"
#include "include/AtlasUtils.hpp"
#include "include/ActionTags.hpp"
#include "core/ActionGroup.hpp"
#include "core/AtlasState.hpp"
#include "core/AtlasInst.hpp"

namespace atlas
{
    template <typename XLEN>
    void RviInsts::getInstComputeAddressHandlers(std::map<std::string, Action> & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same<XLEN, RV32>::value);
        if constexpr (std::is_same_v<XLEN, RV64>)
        {
            inst_handlers.emplace(
                "lb",
                atlas::Action::createAction<&RviInsts::lb_64_compute_address_handler, RviInsts>(
                    nullptr, "lb", ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "lbu",
                atlas::Action::createAction<&RviInsts::lbu_64_compute_address_handler, RviInsts>(
                    nullptr, "lbu", ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "ld",
                atlas::Action::createAction<&RviInsts::ld_64_compute_address_handler, RviInsts>(
                    nullptr, "ld", ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "lh",
                atlas::Action::createAction<&RviInsts::lh_64_compute_address_handler, RviInsts>(
                    nullptr, "lh", ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "lhu",
                atlas::Action::createAction<&RviInsts::lhu_64_compute_address_handler, RviInsts>(
                    nullptr, "lhu", ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "lw",
                atlas::Action::createAction<&RviInsts::lw_64_compute_address_handler, RviInsts>(
                    nullptr, "lw", ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "lwu",
                atlas::Action::createAction<&RviInsts::lwu_64_compute_address_handler, RviInsts>(
                    nullptr, "lwu", ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "sb",
                atlas::Action::createAction<&RviInsts::sb_64_compute_address_handler, RviInsts>(
                    nullptr, "sb", ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "sd",
                atlas::Action::createAction<&RviInsts::sd_64_compute_address_handler, RviInsts>(
                    nullptr, "sd", ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "sh",
                atlas::Action::createAction<&RviInsts::sh_64_compute_address_handler, RviInsts>(
                    nullptr, "sh", ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "sw",
                atlas::Action::createAction<&RviInsts::sw_64_compute_address_handler, RviInsts>(
                    nullptr, "sw", ActionTags::COMPUTE_ADDR_TAG));
        }
        else if constexpr (std::is_same_v<XLEN, RV32>)
        {
            sparta_assert(false, "RV32 is not supported yet!");
        }
    }

    template <typename XLEN>
    void RviInsts::getInstHandlers(std::map<std::string, Action> & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same<XLEN, RV32>::value);
        if constexpr (std::is_same_v<XLEN, RV64>)
        {
            inst_handlers.emplace("add",
                                  atlas::Action::createAction<&RviInsts::add_64_handler, RviInsts>(
                                      nullptr, "add", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("addi",
                                  atlas::Action::createAction<&RviInsts::addi_64_handler, RviInsts>(
                                      nullptr, "addi", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "addiw", atlas::Action::createAction<&RviInsts::addiw_64_handler, RviInsts>(
                             nullptr, "addiw", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("addw",
                                  atlas::Action::createAction<&RviInsts::addw_64_handler, RviInsts>(
                                      nullptr, "addw", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("and",
                                  atlas::Action::createAction<&RviInsts::and_64_handler, RviInsts>(
                                      nullptr, "and", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("andi",
                                  atlas::Action::createAction<&RviInsts::andi_64_handler, RviInsts>(
                                      nullptr, "andi", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "auipc", atlas::Action::createAction<&RviInsts::auipc_64_handler, RviInsts>(
                             nullptr, "auipc", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("beq",
                                  atlas::Action::createAction<&RviInsts::beq_64_handler, RviInsts>(
                                      nullptr, "beq", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("bge",
                                  atlas::Action::createAction<&RviInsts::bge_64_handler, RviInsts>(
                                      nullptr, "bge", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("bgeu",
                                  atlas::Action::createAction<&RviInsts::bgeu_64_handler, RviInsts>(
                                      nullptr, "bgeu", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("blt",
                                  atlas::Action::createAction<&RviInsts::blt_64_handler, RviInsts>(
                                      nullptr, "blt", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("bltu",
                                  atlas::Action::createAction<&RviInsts::bltu_64_handler, RviInsts>(
                                      nullptr, "bltu", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("bne",
                                  atlas::Action::createAction<&RviInsts::bne_64_handler, RviInsts>(
                                      nullptr, "bne", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "cdiscard.d.l1",
                atlas::Action::createAction<&RviInsts::cdiscard_d_l1_64_handler, RviInsts>(
                    nullptr, "cdiscard_d_l1", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "cflush.d.l1",
                atlas::Action::createAction<&RviInsts::cflush_d_l1_64_handler, RviInsts>(
                    nullptr, "cflush_d_l1", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "cflush.i.l1",
                atlas::Action::createAction<&RviInsts::cflush_i_l1_64_handler, RviInsts>(
                    nullptr, "cflush_i_l1", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("dret",
                                  atlas::Action::createAction<&RviInsts::dret_64_handler, RviInsts>(
                                      nullptr, "dret", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "ebreak", atlas::Action::createAction<&RviInsts::ebreak_64_handler, RviInsts>(
                              nullptr, "ebreak", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "ecall", atlas::Action::createAction<&RviInsts::ecall_64_handler, RviInsts>(
                             nullptr, "ecall", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "fence", atlas::Action::createAction<&RviInsts::fence_64_handler, RviInsts>(
                             nullptr, "fence", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("jal",
                                  atlas::Action::createAction<&RviInsts::jal_64_handler, RviInsts>(
                                      nullptr, "jal", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("jalr",
                                  atlas::Action::createAction<&RviInsts::jalr_64_handler, RviInsts>(
                                      nullptr, "jalr", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("lb",
                                  atlas::Action::createAction<&RviInsts::lb_64_handler, RviInsts>(
                                      nullptr, "lb", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("lbu",
                                  atlas::Action::createAction<&RviInsts::lbu_64_handler, RviInsts>(
                                      nullptr, "lbu", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("ld",
                                  atlas::Action::createAction<&RviInsts::ld_64_handler, RviInsts>(
                                      nullptr, "ld", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("lh",
                                  atlas::Action::createAction<&RviInsts::lh_64_handler, RviInsts>(
                                      nullptr, "lh", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("lhu",
                                  atlas::Action::createAction<&RviInsts::lhu_64_handler, RviInsts>(
                                      nullptr, "lhu", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("li",
                                  atlas::Action::createAction<&RviInsts::li_64_handler, RviInsts>(
                                      nullptr, "li", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("lui",
                                  atlas::Action::createAction<&RviInsts::lui_64_handler, RviInsts>(
                                      nullptr, "lui", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("lw",
                                  atlas::Action::createAction<&RviInsts::lw_64_handler, RviInsts>(
                                      nullptr, "lw", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("lwu",
                                  atlas::Action::createAction<&RviInsts::lwu_64_handler, RviInsts>(
                                      nullptr, "lwu", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("mret",
                                  atlas::Action::createAction<&RviInsts::mret_64_handler, RviInsts>(
                                      nullptr, "mret", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("mv",
                                  atlas::Action::createAction<&RviInsts::mv_64_handler, RviInsts>(
                                      nullptr, "mv", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("nop",
                                  atlas::Action::createAction<&RviInsts::nop_64_handler, RviInsts>(
                                      nullptr, "nop", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("or",
                                  atlas::Action::createAction<&RviInsts::or_64_handler, RviInsts>(
                                      nullptr, "or", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("ori",
                                  atlas::Action::createAction<&RviInsts::ori_64_handler, RviInsts>(
                                      nullptr, "ori", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("sb",
                                  atlas::Action::createAction<&RviInsts::sb_64_handler, RviInsts>(
                                      nullptr, "sb", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("sd",
                                  atlas::Action::createAction<&RviInsts::sd_64_handler, RviInsts>(
                                      nullptr, "sd", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "sfence.vma",
                atlas::Action::createAction<&RviInsts::sfence_vma_64_handler, RviInsts>(
                    nullptr, "sfence_vma", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("sh",
                                  atlas::Action::createAction<&RviInsts::sh_64_handler, RviInsts>(
                                      nullptr, "sh", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("sll",
                                  atlas::Action::createAction<&RviInsts::sll_64_handler, RviInsts>(
                                      nullptr, "sll", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("slli",
                                  atlas::Action::createAction<&RviInsts::slli_64_handler, RviInsts>(
                                      nullptr, "slli", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "slliw", atlas::Action::createAction<&RviInsts::slliw_64_handler, RviInsts>(
                             nullptr, "slliw", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("sllw",
                                  atlas::Action::createAction<&RviInsts::sllw_64_handler, RviInsts>(
                                      nullptr, "sllw", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("slt",
                                  atlas::Action::createAction<&RviInsts::slt_64_handler, RviInsts>(
                                      nullptr, "slt", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("slti",
                                  atlas::Action::createAction<&RviInsts::slti_64_handler, RviInsts>(
                                      nullptr, "slti", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "sltiu", atlas::Action::createAction<&RviInsts::sltiu_64_handler, RviInsts>(
                             nullptr, "sltiu", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("sltu",
                                  atlas::Action::createAction<&RviInsts::sltu_64_handler, RviInsts>(
                                      nullptr, "sltu", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("sra",
                                  atlas::Action::createAction<&RviInsts::sra_64_handler, RviInsts>(
                                      nullptr, "sra", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("srai",
                                  atlas::Action::createAction<&RviInsts::srai_64_handler, RviInsts>(
                                      nullptr, "srai", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "sraiw", atlas::Action::createAction<&RviInsts::sraiw_64_handler, RviInsts>(
                             nullptr, "sraiw", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("sraw",
                                  atlas::Action::createAction<&RviInsts::sraw_64_handler, RviInsts>(
                                      nullptr, "sraw", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("sret",
                                  atlas::Action::createAction<&RviInsts::sret_64_handler, RviInsts>(
                                      nullptr, "sret", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("srl",
                                  atlas::Action::createAction<&RviInsts::srl_64_handler, RviInsts>(
                                      nullptr, "srl", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("srli",
                                  atlas::Action::createAction<&RviInsts::srli_64_handler, RviInsts>(
                                      nullptr, "srli", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "srliw", atlas::Action::createAction<&RviInsts::srliw_64_handler, RviInsts>(
                             nullptr, "srliw", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("srlw",
                                  atlas::Action::createAction<&RviInsts::srlw_64_handler, RviInsts>(
                                      nullptr, "srlw", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("sub",
                                  atlas::Action::createAction<&RviInsts::sub_64_handler, RviInsts>(
                                      nullptr, "sub", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("subw",
                                  atlas::Action::createAction<&RviInsts::subw_64_handler, RviInsts>(
                                      nullptr, "subw", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("sw",
                                  atlas::Action::createAction<&RviInsts::sw_64_handler, RviInsts>(
                                      nullptr, "sw", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("uret",
                                  atlas::Action::createAction<&RviInsts::uret_64_handler, RviInsts>(
                                      nullptr, "uret", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("wfi",
                                  atlas::Action::createAction<&RviInsts::wfi_64_handler, RviInsts>(
                                      nullptr, "wfi", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("xor",
                                  atlas::Action::createAction<&RviInsts::xor_64_handler, RviInsts>(
                                      nullptr, "xor", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("xori",
                                  atlas::Action::createAction<&RviInsts::xori_64_handler, RviInsts>(
                                      nullptr, "xori", ActionTags::EXECUTE_TAG));
        }
        else if constexpr (std::is_same_v<XLEN, RV32>)
        {
            sparta_assert(false, "RV32 is not supported yet!");
        }
    }

    template void RviInsts::getInstComputeAddressHandlers<RV32>(std::map<std::string, Action> &);
    template void RviInsts::getInstComputeAddressHandlers<RV64>(std::map<std::string, Action> &);
    template void RviInsts::getInstHandlers<RV32>(std::map<std::string, Action> &);
    template void RviInsts::getInstHandlers<RV64>(std::map<std::string, Action> &);

    ActionGroup* RviInsts::cflush_i_l1_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        return nullptr;
    }

    ActionGroup* RviInsts::subw_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        const uint64_t rs1_val = insn->getRs1()->dmiRead<uint64_t>();
        const uint64_t rs2_val = insn->getRs2()->dmiRead<uint64_t>();
        // Casting from int32_t to int64_t will sign extend the value
        const uint64_t rd_val = ((int64_t)(int32_t)(rs1_val - rs2_val));
        insn->getRd()->dmiWrite(rd_val);

        return nullptr;
    }

    ActionGroup* RviInsts::slt_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        const int64_t rs1_val = insn->getRs1()->dmiRead<uint64_t>();
        const int64_t rs2_val = insn->getRs2()->dmiRead<uint64_t>();
        const int64_t rd_val = rs1_val < rs2_val;
        insn->getRd()->dmiWrite(rd_val);

        return nullptr;
    }

    ActionGroup* RviInsts::sh_64_compute_address_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();
        const uint64_t rs1_val = insn->getRs1()->dmiRead<uint64_t>();
        constexpr uint64_t IMM_SIZE = 12;
        const uint64_t imm = insn->getSignExtendedImmediate<RV64, IMM_SIZE>();
        const uint64_t vaddr = rs1_val + imm;
        state->getTranslationState()->makeTranslationRequest(vaddr, sizeof(uint16_t));
        return nullptr;
    }

    ActionGroup* RviInsts::sh_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();
        const uint64_t rs2_val = insn->getRs2()->dmiRead<uint64_t>();
        const uint64_t paddr = state->getTranslationState()->getTranslationResult().getPaddr();
        state->writeMemory<uint16_t>(paddr, rs2_val);
        return nullptr;
    }

    ActionGroup* RviInsts::sltu_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        const uint64_t rs1_val = insn->getRs1()->dmiRead<uint64_t>();
        const uint64_t rs2_val = insn->getRs2()->dmiRead<uint64_t>();
        const uint64_t rd_val = rs1_val < rs2_val;
        insn->getRd()->dmiWrite(rd_val);

        return nullptr;
    }

    ActionGroup* RviInsts::mret_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_privilege(PRV_M);
        // set_pc_and_serialize(p->get_state()->mepc->read());
        // reg_t s = STATE.mstatus->read();
        // reg_t prev_prv = get_field(s, MSTATUS_MPP);
        // reg_t prev_virt = get_field(s, MSTATUS_MPV);
        // if (prev_prv != PRV_M)
        //   s = set_field(s, MSTATUS_MPRV, 0);
        // s = set_field(s, MSTATUS_MIE, get_field(s, MSTATUS_MPIE));
        // s = set_field(s, MSTATUS_MPIE, 1);
        // s = set_field(s, MSTATUS_MPP, p->extension_enabled('U') ? PRV_U : PRV_M);
        // s = set_field(s, MSTATUS_MPV, 0);
        // if (ZICFILP_xLPE(prev_virt, prev_prv)) {
        //   STATE.elp = static_cast<elp_t>(get_field(s, MSTATUS_MPELP));
        // }
        // s = set_field(s, MSTATUS_MPELP, elp_t::NO_LP_EXPECTED);
        // s = set_field(s, MSTATUS_MDT, 0);
        // if (prev_prv == PRV_U || prev_virt)
        //   s = set_field(s, MSTATUS_SDT, 0);
        // if (prev_virt && prev_prv == PRV_U)
        //   STATE.vsstatus->write(STATE.vsstatus->read() & ~SSTATUS_SDT);
        // STATE.mstatus->write(s);
        // if (STATE.mstatush) STATE.mstatush->write(s >> 32); // log mstatush change
        // STATE.tcontrol->write((STATE.tcontrol->read() & CSR_TCONTROL_MPTE) ?
        // (CSR_TCONTROL_MPTE | CSR_TCONTROL_MTE) : 0); p->set_privilege(prev_prv,
        // prev_virt);

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////

        return nullptr;
    }

    ActionGroup* RviInsts::sb_64_compute_address_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();
        const uint64_t rs1_val = insn->getRs1()->dmiRead<uint64_t>();
        constexpr uint64_t IMM_SIZE = 12;
        const uint64_t imm = insn->getSignExtendedImmediate<RV64, IMM_SIZE>();
        const uint64_t vaddr = rs1_val + imm;
        state->getTranslationState()->makeTranslationRequest(vaddr, sizeof(uint8_t));
        return nullptr;
    }

    ActionGroup* RviInsts::sb_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();
        const uint64_t rs2_val = insn->getRs2()->dmiRead<uint64_t>();
        const uint64_t paddr = state->getTranslationState()->getTranslationResult().getPaddr();
        state->writeMemory<uint8_t>(paddr, rs2_val);
        return nullptr;
    }

    ActionGroup* RviInsts::wfi_64_handler(atlas::AtlasState* state)
    {
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // if (get_field(STATE.mstatus->read(), MSTATUS_TW)) {
        //   require_privilege(PRV_M);
        // } else if (STATE.v) {
        //   if (STATE.prv == PRV_U || get_field(STATE.hstatus->read(), HSTATUS_VTW))
        //     require_novirt();
        // } else if (p->extension_enabled('S')) {
        //   // When S-mode is implemented, then executing WFI in
        //   // U-mode causes an illegal instruction exception.
        //   require_privilege(PRV_S);
        // }
        // wfi();

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////

        if (state->getStopSimOnWfi())
        {
            AtlasState::SimState* sim_state = state->getSimState();
            sim_state->sim_stopped = true;
            return state->getStopSimActionGroup();
        }
        return nullptr;
    }

    ActionGroup* RviInsts::lhu_64_compute_address_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();
        const uint64_t rs1_val = insn->getRs1()->dmiRead<uint64_t>();
        constexpr uint64_t IMM_SIZE = 12;
        const uint64_t imm = insn->getSignExtendedImmediate<RV64, IMM_SIZE>();
        const uint64_t vaddr = rs1_val + imm;
        state->getTranslationState()->makeTranslationRequest(vaddr, sizeof(uint16_t));
        return nullptr;
    }

    ActionGroup* RviInsts::lhu_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();
        const uint64_t paddr = state->getTranslationState()->getTranslationResult().getPaddr();
        const uint64_t rd_val = state->readMemory<uint16_t>(paddr);
        insn->getRd()->dmiWrite(rd_val);
        return nullptr;
    }

    ActionGroup* RviInsts::xori_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        const uint32_t IMM_SIZE = 12;
        const uint64_t imm = insn->getSignExtendedImmediate<RV64, IMM_SIZE>();
        const uint64_t rs1_val = insn->getRs1()->dmiRead<uint64_t>();
        const uint64_t rd_val = rs1_val ^ imm;
        insn->getRd()->dmiWrite(rd_val);

        return nullptr;
    }

    ActionGroup* RviInsts::sw_64_compute_address_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();
        const uint64_t rs1_val = insn->getRs1()->dmiRead<uint64_t>();
        constexpr uint64_t IMM_SIZE = 12;
        const uint64_t imm = insn->getSignExtendedImmediate<RV64, IMM_SIZE>();
        const uint64_t vaddr = rs1_val + imm;
        state->getTranslationState()->makeTranslationRequest(vaddr, sizeof(uint32_t));
        return nullptr;
    }

    ActionGroup* RviInsts::sw_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();
        const uint64_t rs2_val = insn->getRs2()->dmiRead<uint64_t>();
        const uint64_t paddr = state->getTranslationState()->getTranslationResult().getPaddr();
        state->writeMemory<uint32_t>(paddr, rs2_val);
        return nullptr;
    }

    ActionGroup* RviInsts::ecall_64_handler(atlas::AtlasState* state)
    {
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // switch (STATE.prv)
        //{
        //   case PRV_U: throw trap_user_ecall();
        //   case PRV_S:
        //     if (STATE.v)
        //       throw trap_virtual_supervisor_ecall();
        //     else
        //       throw trap_supervisor_ecall();
        //   case PRV_M: throw trap_machine_ecall();
        //   default: abort();
        // }

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////

        // Command
        const uint64_t cmd = state->getIntRegister(17)->dmiRead<uint64_t>(); // a7

        // Only support exit for now so we can end simulation
        if (cmd == 93)
        {
            // Function arguments are a0-a6 (x10-x16)
            const uint64_t exit_code = state->getIntRegister(10)->dmiRead<uint64_t>();

            AtlasState::SimState* sim_state = state->getSimState();
            sim_state->workload_exit_code = exit_code;
            sim_state->test_passed = (exit_code == 0) ? true : false;
            sim_state->sim_stopped = true;

            // Stop simulation
            return state->getStopSimActionGroup();
        }

        return nullptr;
    }

    ActionGroup* RviInsts::sfence_vma_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_extension('S');
        // require_impl(IMPL_MMU);
        // if (STATE.v) {
        //   if (STATE.prv == PRV_U || get_field(STATE.hstatus->read(), HSTATUS_VTVM))
        //     require_novirt();
        // } else {
        //   require_privilege(get_field(STATE.mstatus->read(), MSTATUS_TVM) ? PRV_M :
        //   PRV_S);
        // }
        // MMU.flush_tlb();

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////

        return nullptr;
    }

    ActionGroup* RviInsts::dret_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require(STATE.debug_mode);
        // set_pc_and_serialize(STATE.dpc->read());
        // if (ZICFILP_xLPE(STATE.dcsr->v, STATE.dcsr->prv)) {
        //   STATE.elp = STATE.dcsr->pelp;
        // }
        // p->set_privilege(STATE.dcsr->prv, STATE.dcsr->v);
        // if (STATE.prv < PRV_M) {
        //   STATE.mstatus->write(STATE.mstatus->read() & ~MSTATUS_MPRV);
        //   STATE.mstatus->write(STATE.mstatus->read() & ~MSTATUS_MDT);
        // }
        //
        // if (STATE.dcsr->prv == PRV_U || STATE.dcsr->v)
        //   STATE.mstatus->write(STATE.mstatus->read() & ~MSTATUS_SDT);
        //
        // if (STATE.dcsr->v && STATE.dcsr->prv == PRV_U)
        //   STATE.vsstatus->write(STATE.vsstatus->read() & ~SSTATUS_SDT);
        //
        // if (STATE.dcsr->prv == PRV_U || STATE.dcsr->v)
        //   STATE.mstatus->write(STATE.mstatus->read() & ~MSTATUS_SDT);
        //
        // if (STATE.dcsr->v && STATE.dcsr->prv == PRV_U)
        //   STATE.vsstatus->write(STATE.vsstatus->read() & ~SSTATUS_SDT);
        //
        ///* We're not in Debug Mode anymore. */
        // STATE.debug_mode = false;
        //
        // if (STATE.dcsr->step)
        //   STATE.single_step = STATE.STEP_STEPPING;

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////

        return nullptr;
    }

    ActionGroup* RviInsts::addw_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        const uint64_t rs1_val = insn->getRs1()->dmiRead<uint64_t>();
        const uint64_t rs2_val = insn->getRs2()->dmiRead<uint64_t>();
        // Casting from int32_t to int64_t will sign extend the value
        const uint64_t rd_val = ((int64_t)(int32_t)(rs1_val + rs2_val));
        insn->getRd()->dmiWrite(rd_val);

        return nullptr;
    }

    ActionGroup* RviInsts::xor_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        const uint64_t rs1_val = insn->getRs1()->dmiRead<uint64_t>();
        const uint64_t rs2_val = insn->getRs2()->dmiRead<uint64_t>();
        const uint64_t rd_val = rs1_val ^ rs2_val;
        insn->getRd()->dmiWrite(rd_val);

        return nullptr;
    }

    ActionGroup* RviInsts::bltu_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        const uint64_t rs1_val = insn->getRs1()->dmiRead<uint64_t>();
        const uint64_t rs2_val = insn->getRs2()->dmiRead<uint64_t>();

        if (rs1_val < rs2_val)
        {
            const uint64_t pc = state->getPc();
            const uint32_t IMM_SIZE = 13;
            const uint64_t imm = insn->getSignExtendedImmediate<RV64, IMM_SIZE>();
            const uint64_t branch_target = pc + imm;
            state->setNextPc(branch_target);
        }

        return nullptr;
    }

    ActionGroup* RviInsts::lb_64_compute_address_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();
        const uint64_t rs1_val = insn->getRs1()->dmiRead<uint64_t>();
        constexpr uint64_t IMM_SIZE = 12;
        const uint64_t imm = insn->getSignExtendedImmediate<RV64, IMM_SIZE>();
        const uint64_t vaddr = rs1_val + imm;
        state->getTranslationState()->makeTranslationRequest(vaddr, sizeof(uint8_t));
        return nullptr;
    }

    ActionGroup* RviInsts::lb_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();
        const uint64_t paddr = state->getTranslationState()->getTranslationResult().getPaddr();
        const uint64_t rd_val = signExtend<uint8_t, uint64_t>(state->readMemory<uint8_t>(paddr));
        insn->getRd()->dmiWrite(rd_val);
        return nullptr;
    }

    ActionGroup* RviInsts::mv_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        const uint64_t rs1_val = insn->getRs1()->dmiRead<uint64_t>();
        insn->getRd()->dmiWrite(rs1_val);

        return nullptr;
    }

    ActionGroup* RviInsts::jalr_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        // CHECK_RD();
        uint64_t rd_val = state->getPc() + insn->getOpcodeSize();
        const uint64_t rs1_val = insn->getRs1()->dmiRead<uint64_t>();
        constexpr uint32_t IMM_SIZE = 12;
        const uint64_t imm = insn->getSignExtendedImmediate<RV64, IMM_SIZE>();
        const uint64_t jump_target = (rs1_val + imm) & ~int64_t(1);
        state->setNextPc(jump_target);
        insn->getRd()->dmiWrite(rd_val);

        // if (ZICFILP_xLPE(STATE.v, STATE.prv)) {
        //     STATE.elp = ZICFILP_IS_LP_EXPECTED(insn.rs1());
        //     serialize();
        // }

        return nullptr;
    }

    ActionGroup* RviInsts::sd_64_compute_address_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();
        const uint64_t rs1_val = insn->getRs1()->dmiRead<uint64_t>();
        constexpr uint64_t IMM_SIZE = 12;
        const uint64_t imm = insn->getSignExtendedImmediate<RV64, IMM_SIZE>();
        const uint64_t vaddr = rs1_val + imm;
        state->getTranslationState()->makeTranslationRequest(vaddr, sizeof(uint64_t));
        return nullptr;
    }

    ActionGroup* RviInsts::sd_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();
        const uint64_t rs2_val = insn->getRs2()->dmiRead<uint64_t>();
        const uint64_t paddr = state->getTranslationState()->getTranslationResult().getPaddr();
        state->writeMemory<uint64_t>(paddr, rs2_val);
        return nullptr;
    }

    ActionGroup* RviInsts::srl_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        const uint64_t rs1_val = insn->getRs1()->dmiRead<uint64_t>();
        const uint64_t rs2_val = insn->getRs2()->dmiRead<uint64_t>();
        const uint64_t rd_val = (int64_t)(rs1_val >> (rs2_val & (state->getXlen() - 1)));
        insn->getRd()->dmiWrite(rd_val);

        return nullptr;
    }

    ActionGroup* RviInsts::bne_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        const int64_t rs1_val = insn->getRs1()->dmiRead<uint64_t>();
        const int64_t rs2_val = insn->getRs2()->dmiRead<uint64_t>();

        if (rs1_val != rs2_val)
        {
            const uint64_t pc = state->getPc();
            const uint32_t IMM_SIZE = 13;
            const uint64_t imm = insn->getSignExtendedImmediate<RV64, IMM_SIZE>();
            const uint64_t branch_target = pc + imm;
            state->setNextPc(branch_target);
        }

        return nullptr;
    }

    ActionGroup* RviInsts::and_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        const uint64_t rs1_val = insn->getRs1()->dmiRead<uint64_t>();
        const uint64_t rs2_val = insn->getRs2()->dmiRead<uint64_t>();
        const uint64_t rd_val = rs1_val & rs2_val;
        insn->getRd()->dmiWrite(rd_val);

        return nullptr;
    }

    ActionGroup* RviInsts::lui_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        const uint64_t imm = insn->getSignExtendedImmediate<RV64, 32>();
        insn->getRd()->dmiWrite(imm);

        return nullptr;
    }

    ActionGroup* RviInsts::auipc_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        const uint32_t IMM_SIZE = 32;
        const uint64_t imm = insn->getSignExtendedImmediate<RV64, IMM_SIZE>();
        const uint64_t pc = state->getPc();
        const uint64_t rd_val =
            ((int64_t)(imm + pc) << (64 - (state->getXlen()))) >> (64 - (state->getXlen()));
        insn->getRd()->dmiWrite(rd_val);

        return nullptr;
    }

    ActionGroup* RviInsts::sltiu_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        const uint64_t rs1_val = insn->getRs1()->dmiRead<uint64_t>();
        const uint32_t IMM_SIZE = 12;
        const uint64_t imm = insn->getSignExtendedImmediate<RV64, IMM_SIZE>();
        const uint64_t rd_val = rs1_val < imm;
        insn->getRd()->dmiWrite(rd_val);

        return nullptr;
    }

    ActionGroup* RviInsts::ori_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        // prefetch.i/r/w hint when rd = 0 and i_imm[4:0] = 0/1/3
        const uint32_t IMM_SIZE = 12;
        const uint64_t imm = insn->getSignExtendedImmediate<RV64, IMM_SIZE>();
        const uint64_t rs1_val = insn->getRs1()->dmiRead<uint64_t>();
        const uint64_t rd_val = rs1_val | imm;
        insn->getRd()->dmiWrite(rd_val);

        return nullptr;
    }

    ActionGroup* RviInsts::blt_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        const int64_t rs1_val = insn->getRs1()->dmiRead<uint64_t>();
        const int64_t rs2_val = insn->getRs2()->dmiRead<uint64_t>();

        if (rs1_val < rs2_val)
        {
            const uint64_t pc = state->getPc();
            const uint32_t IMM_SIZE = 13;
            const uint64_t imm = insn->getSignExtendedImmediate<RV64, IMM_SIZE>();
            const uint64_t branch_target = pc + imm;
            state->setNextPc(branch_target);
        }

        return nullptr;
    }

    ActionGroup* RviInsts::lh_64_compute_address_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();
        const uint64_t rs1_val = insn->getRs1()->dmiRead<uint64_t>();
        constexpr uint64_t IMM_SIZE = 12;
        const uint64_t imm = insn->getSignExtendedImmediate<RV64, IMM_SIZE>();
        const uint64_t vaddr = rs1_val + imm;
        state->getTranslationState()->makeTranslationRequest(vaddr, sizeof(uint16_t));
        return nullptr;
    }

    ActionGroup* RviInsts::lh_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();
        const uint64_t paddr = state->getTranslationState()->getTranslationResult().getPaddr();
        const uint64_t rd_val = signExtend<uint16_t, uint64_t>(state->readMemory<uint16_t>(paddr));
        insn->getRd()->dmiWrite(rd_val);
        return nullptr;
    }

    ActionGroup* RviInsts::srliw_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        const uint32_t rs1_val = insn->getRs1()->dmiRead<uint64_t>();
        const uint64_t shift_amount = insn->getImmediate() & (state->getXlen() - 1);
        // Casting from int32_t to int64_t will sign extend the value
        const uint64_t rd_val = (int64_t)(int32_t)(rs1_val >> shift_amount);
        insn->getRd()->dmiWrite(rd_val);

        return nullptr;
    }

    ActionGroup* RviInsts::addi_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        const uint64_t rs1_val = insn->getRs1()->dmiRead<uint64_t>();
        const uint32_t IMM_SIZE = 12;
        const uint64_t imm = insn->getSignExtendedImmediate<RV64, IMM_SIZE>();
        const uint64_t rd_val = rs1_val + imm;
        insn->getRd()->dmiWrite(rd_val);

        return nullptr;
    }

    ActionGroup* RviInsts::nop_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        return nullptr;
    }

    ActionGroup* RviInsts::sll_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        const uint64_t rs1_val = insn->getRs1()->dmiRead<uint64_t>();
        const uint64_t rs2_val = insn->getRs2()->dmiRead<uint64_t>();
        const uint64_t rd_val = rs1_val << (rs2_val & (state->getXlen() - 1));
        insn->getRd()->dmiWrite(rd_val);

        return nullptr;
    }

    ActionGroup* RviInsts::add_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        const uint64_t rs1_val = insn->getRs1()->dmiRead<uint64_t>();
        const uint64_t rs2_val = insn->getRs2()->dmiRead<uint64_t>();
        const uint64_t rd_val = rs1_val + rs2_val;
        insn->getRd()->dmiWrite(rd_val);

        return nullptr;
    }

    ActionGroup* RviInsts::ld_64_compute_address_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();
        const uint64_t rs1_val = insn->getRs1()->dmiRead<uint64_t>();
        constexpr uint64_t IMM_SIZE = 12;
        const uint64_t imm = insn->getSignExtendedImmediate<RV64, IMM_SIZE>();
        const uint64_t vaddr = rs1_val + imm;
        state->getTranslationState()->makeTranslationRequest(vaddr, sizeof(uint64_t));
        return nullptr;
    }

    ActionGroup* RviInsts::ld_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();
        const uint64_t paddr = state->getTranslationState()->getTranslationResult().getPaddr();
        const uint64_t rd_val = state->readMemory<uint64_t>(paddr);
        insn->getRd()->dmiWrite(rd_val);
        return nullptr;
    }

    ActionGroup* RviInsts::srai_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        // require(SHAMT < state->getXlen());
        const int64_t rs1_val = insn->getRs1()->dmiRead<uint64_t>();
        const uint64_t shift_amount = insn->getImmediate() & (state->getXlen() - 1);
        const int64_t rd_val = (int64_t)(rs1_val >> shift_amount);
        insn->getRd()->dmiWrite(rd_val);

        return nullptr;
    }

    ActionGroup* RviInsts::sraw_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        const int32_t rs1_val = insn->getRs1()->dmiRead<uint64_t>();
        const uint64_t rs2_val = insn->getRs2()->dmiRead<uint64_t>();
        // Casting from int32_t to int64_t will sign extend the value
        const uint64_t rd_val = (int64_t)(int32_t)(rs1_val >> (rs2_val & 0x1F));
        insn->getRd()->dmiWrite(rd_val);

        return nullptr;
    }

    ActionGroup* RviInsts::andi_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        const uint32_t IMM_SIZE = 12;
        const uint64_t imm = insn->getSignExtendedImmediate<RV64, IMM_SIZE>();
        const uint64_t rs1_val = insn->getRs1()->dmiRead<uint64_t>();
        const uint64_t rd_val = imm & rs1_val;
        insn->getRd()->dmiWrite(rd_val);

        return nullptr;
    }

    ActionGroup* RviInsts::lw_64_compute_address_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();
        const uint64_t rs1_val = insn->getRs1()->dmiRead<uint64_t>();
        constexpr uint64_t IMM_SIZE = 12;
        const uint64_t imm = insn->getSignExtendedImmediate<RV64, IMM_SIZE>();
        const uint64_t vaddr = rs1_val + imm;
        state->getTranslationState()->makeTranslationRequest(vaddr, sizeof(uint32_t));
        return nullptr;
    }

    ActionGroup* RviInsts::lw_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();
        const uint64_t paddr = state->getTranslationState()->getTranslationResult().getPaddr();
        const uint64_t rd_val = signExtend<uint32_t, uint64_t>(state->readMemory<uint32_t>(paddr));
        insn->getRd()->dmiWrite(rd_val);
        return nullptr;
    }

    ActionGroup* RviInsts::li_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        const uint32_t IMM_SIZE = 12;
        const uint64_t imm = insn->getSignExtendedImmediate<RV64, IMM_SIZE>();
        insn->getRd()->dmiWrite(imm);

        return nullptr;
    }

    ActionGroup* RviInsts::sret_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_extension('S');
        // reg_t prev_hstatus = STATE.hstatus->read();
        // if (STATE.v) {
        //   if (STATE.prv == PRV_U || get_field(prev_hstatus, HSTATUS_VTSR))
        //     require_novirt();
        // } else {
        //   require_privilege(get_field(STATE.mstatus->read(), MSTATUS_TSR) ? PRV_M :
        //   PRV_S);
        // }
        // reg_t next_pc = p->get_state()->sepc->read();
        // set_pc_and_serialize(next_pc);
        // reg_t s = STATE.sstatus->read();
        // reg_t prev_prv = get_field(s, MSTATUS_SPP);
        // s = set_field(s, MSTATUS_SIE, get_field(s, MSTATUS_SPIE));
        // s = set_field(s, MSTATUS_SPIE, 1);
        // s = set_field(s, MSTATUS_SPP, PRV_U);
        // bool prev_virt = STATE.v;
        // if (!STATE.v) {
        //   if (p->extension_enabled('H')) {
        //     prev_virt = get_field(prev_hstatus, HSTATUS_SPV);
        //     reg_t new_hstatus = set_field(prev_hstatus, HSTATUS_SPV, 0);
        //     STATE.hstatus->write(new_hstatus);
        //   }
        //
        //   STATE.mstatus->write(set_field(STATE.mstatus->read(), MSTATUS_MPRV, 0));
        // }
        // if (ZICFILP_xLPE(prev_virt, prev_prv)) {
        //   STATE.elp = static_cast<elp_t>(get_field(s, SSTATUS_SPELP));
        // }
        //
        // if (STATE.prv == PRV_M) {
        //   STATE.mstatus->write(STATE.mstatus->read() & ~MSTATUS_MDT);
        //   if (prev_prv == PRV_U || prev_virt)
        //     STATE.mstatus->write(STATE.mstatus->read() & ~MSTATUS_SDT);
        //   if (prev_virt && prev_prv == PRV_U)
        //     STATE.vsstatus->write(STATE.vsstatus->read() & ~SSTATUS_SDT);
        // }
        //
        // s = set_field(s, SSTATUS_SPELP, elp_t::NO_LP_EXPECTED);
        //
        // if (STATE.prv == PRV_S) {
        //   s = set_field(s, SSTATUS_SDT, 0);
        //   if (!STATE.v && prev_virt && prev_prv == PRV_U)
        //     STATE.vsstatus->write(STATE.vsstatus->read() & ~SSTATUS_SDT);
        // }
        //
        // STATE.sstatus->write(s);
        // p->set_privilege(prev_prv, prev_virt);

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////

        return nullptr;
    }

    ActionGroup* RviInsts::bge_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        const int64_t rs1_val = insn->getRs1()->dmiRead<uint64_t>();
        const int64_t rs2_val = insn->getRs2()->dmiRead<uint64_t>();

        if (rs1_val >= rs2_val)
        {
            const uint64_t pc = state->getPc();
            const uint32_t IMM_SIZE = 13;
            const uint64_t imm = insn->getSignExtendedImmediate<RV64, IMM_SIZE>();
            const uint64_t branch_target = pc + imm;
            state->setNextPc(branch_target);
        }

        return nullptr;
    }

    ActionGroup* RviInsts::slti_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        const int64_t rs1_val = insn->getRs1()->dmiRead<uint64_t>();
        const uint32_t IMM_SIZE = 12;
        const int64_t imm = insn->getSignExtendedImmediate<RV64, IMM_SIZE>();
        const int64_t rd_val = rs1_val < imm;
        insn->getRd()->dmiWrite(rd_val);

        return nullptr;
    }

    ActionGroup* RviInsts::bgeu_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        const uint64_t rs1_val = insn->getRs1()->dmiRead<uint64_t>();
        const uint64_t rs2_val = insn->getRs2()->dmiRead<uint64_t>();

        if (rs1_val >= rs2_val)
        {
            const uint64_t pc = state->getPc();
            const uint32_t IMM_SIZE = 13;
            const uint64_t imm = insn->getSignExtendedImmediate<RV64, IMM_SIZE>();
            const uint64_t branch_target = pc + imm;
            state->setNextPc(branch_target);
        }

        return nullptr;
    }

    ActionGroup* RviInsts::jal_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        // CHECK_RD();
        int64_t rd_val = state->getPc() + insn->getOpcodeSize();
        const uint64_t jump_target = state->getPc() + insn->getImmediate();
        state->setNextPc(jump_target);
        insn->getRd()->dmiWrite(rd_val);

        return nullptr;
    }

    ActionGroup* RviInsts::or_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        const uint64_t rs1_val = insn->getRs1()->dmiRead<uint64_t>();
        const uint64_t rs2_val = insn->getRs2()->dmiRead<uint64_t>();
        const uint64_t rd_val = rs1_val | rs2_val;
        insn->getRd()->dmiWrite(rd_val);

        return nullptr;
    }

    ActionGroup* RviInsts::srli_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        // require(SHAMT < state->getXlen());
        const uint64_t rs1_val = insn->getRs1()->dmiRead<uint64_t>();
        const uint64_t shift_amount = insn->getImmediate() & (state->getXlen() - 1);
        const int64_t rd_val = (int64_t)(rs1_val >> shift_amount);
        insn->getRd()->dmiWrite(rd_val);

        return nullptr;
    }

    ActionGroup* RviInsts::sllw_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        const uint32_t rs1_val = insn->getRs1()->dmiRead<uint64_t>();
        const uint32_t rs2_val = insn->getRs2()->dmiRead<uint64_t>();
        // Casting from int32_t to int64_t will sign extend the value
        const int64_t rd_val = (int64_t)(int32_t)(rs1_val << (rs2_val & 0x1F));
        insn->getRd()->dmiWrite(rd_val);

        return nullptr;
    }

    ActionGroup* RviInsts::slliw_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        const uint32_t rs1_val = insn->getRs1()->dmiRead<uint64_t>();
        const uint64_t shift_amount = insn->getImmediate() & 0x1F;
        // Casting from int32_t to int64_t will sign extend the value
        const int64_t rd_val = (int64_t)(int32_t)(rs1_val << shift_amount);
        insn->getRd()->dmiWrite(rd_val);

        return nullptr;
    }

    ActionGroup* RviInsts::cflush_d_l1_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        return nullptr;
    }

    ActionGroup* RviInsts::sraiw_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        const int32_t rs1_val = insn->getRs1()->dmiRead<uint64_t>();
        const uint64_t shift_amount = insn->getImmediate() & (state->getXlen() - 1);
        // Casting from int32_t to int64_t will sign extend the value
        const uint64_t rd_val = (int64_t)(int32_t)(rs1_val >> shift_amount);
        insn->getRd()->dmiWrite(rd_val);

        return nullptr;
    }

    ActionGroup* RviInsts::uret_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        return nullptr;
    }

    ActionGroup* RviInsts::lwu_64_compute_address_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();
        const uint64_t rs1_val = insn->getRs1()->dmiRead<uint64_t>();
        constexpr uint64_t IMM_SIZE = 12;
        const uint64_t imm = insn->getSignExtendedImmediate<RV64, IMM_SIZE>();
        const uint64_t vaddr = rs1_val + imm;
        state->getTranslationState()->makeTranslationRequest(vaddr, sizeof(uint32_t));
        return nullptr;
    }

    ActionGroup* RviInsts::lwu_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();
        const uint64_t paddr = state->getTranslationState()->getTranslationResult().getPaddr();
        const uint64_t rd_val = state->readMemory<uint32_t>(paddr);
        insn->getRd()->dmiWrite(rd_val);
        return nullptr;
    }

    ActionGroup* RviInsts::sub_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        const uint64_t rs1_val = insn->getRs1()->dmiRead<uint64_t>();
        const uint64_t rs2_val = insn->getRs2()->dmiRead<uint64_t>();
        const uint64_t rd_val = rs1_val - rs2_val;
        insn->getRd()->dmiWrite(rd_val);

        return nullptr;
    }

    ActionGroup* RviInsts::cdiscard_d_l1_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        return nullptr;
    }

    ActionGroup* RviInsts::beq_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        const int64_t rs1_val = insn->getRs1()->dmiRead<uint64_t>();
        const int64_t rs2_val = insn->getRs2()->dmiRead<uint64_t>();

        if (rs1_val == rs2_val)
        {
            const uint64_t pc = state->getPc();
            const uint32_t IMM_SIZE = 13;
            const uint64_t imm = insn->getSignExtendedImmediate<RV64, IMM_SIZE>();
            const uint64_t branch_target = pc + imm;
            state->setNextPc(branch_target);
        }

        return nullptr;
    }

    ActionGroup* RviInsts::sra_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        const int64_t rs1_val = insn->getRs1()->dmiRead<uint64_t>();
        const uint64_t rs2_val = insn->getRs2()->dmiRead<uint64_t>();
        const uint64_t rd_val = (int64_t)(rs1_val >> (rs2_val & (state->getXlen() - 1)));
        insn->getRd()->dmiWrite(rd_val);

        return nullptr;
    }

    ActionGroup* RviInsts::fence_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        return nullptr;
    }

    ActionGroup* RviInsts::slli_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        // require(SHAMT < state->getXlen());
        const uint64_t rs1_val = insn->getRs1()->dmiRead<uint64_t>();
        const uint64_t shift_amount = insn->getImmediate() & (state->getXlen() - 1);
        const uint64_t rd_val = rs1_val << shift_amount;
        insn->getRd()->dmiWrite(rd_val);

        return nullptr;
    }

    ActionGroup* RviInsts::ebreak_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // if (!STATE.debug_mode && (
        //         (!STATE.v && STATE.prv == PRV_M && STATE.dcsr->ebreakm) ||
        //         (!STATE.v && STATE.prv == PRV_S && STATE.dcsr->ebreaks) ||
        //         (!STATE.v && STATE.prv == PRV_U && STATE.dcsr->ebreaku) ||
        //         (STATE.v && STATE.prv == PRV_S && STATE.dcsr->ebreakvs) ||
        //         (STATE.v && STATE.prv == PRV_U && STATE.dcsr->ebreakvu))) {
        //     throw trap_debug_mode();
        // } else {
        //     throw trap_breakpoint(STATE.v, pc);
        // }

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////

        return nullptr;
    }

    ActionGroup* RviInsts::lbu_64_compute_address_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();
        const uint64_t rs1_val = insn->getRs1()->dmiRead<uint64_t>();
        constexpr uint64_t IMM_SIZE = 12;
        const uint64_t imm = insn->getSignExtendedImmediate<RV64, IMM_SIZE>();
        const uint64_t vaddr = rs1_val + imm;
        state->getTranslationState()->makeTranslationRequest(vaddr, sizeof(uint8_t));
        return nullptr;
    }

    ActionGroup* RviInsts::lbu_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();
        const uint64_t paddr = state->getTranslationState()->getTranslationResult().getPaddr();
        const uint64_t rd_val = state->readMemory<uint8_t>(paddr);
        insn->getRd()->dmiWrite(rd_val);
        return nullptr;
    }

    ActionGroup* RviInsts::addiw_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        const uint32_t IMM_SIZE = 12;
        const uint64_t imm = insn->getSignExtendedImmediate<RV64, IMM_SIZE>();
        const uint32_t rs1_val = insn->getRs1()->dmiRead<uint64_t>();
        // Casting from int32_t to int64_t will sign extend the value
        const uint64_t rd_val = ((int64_t)(int32_t)(rs1_val + imm));
        insn->getRd()->dmiWrite(rd_val);

        return nullptr;
    }

    ActionGroup* RviInsts::srlw_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & insn = state->getCurrentInst();

        const uint32_t rs1_val = insn->getRs1()->dmiRead<uint64_t>();
        const uint64_t rs2_val = insn->getRs2()->dmiRead<uint64_t>();
        // Casting from int32_t to int64_t will sign extend the value
        const uint64_t rd_val = (int64_t)(int32_t)(rs1_val >> (rs2_val & 0x1F));
        insn->getRd()->dmiWrite(rd_val);

        return nullptr;
    }

} // namespace atlas
