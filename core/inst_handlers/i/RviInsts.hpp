#pragma once

#include "include/AtlasTypes.hpp"

#include <map>
#include <string>

namespace atlas
{
    class AtlasState;
    class Action;
    class ActionGroup;

    class RviInsts
    {
      public:
        using base_type = RviInsts;

        template <typename XLEN>
        static void getInstComputeAddressHandlers(std::map<std::string, Action> &);
        template <typename XLEN> static void getInstHandlers(std::map<std::string, Action> &);

      private:
        // add,slt,sltu,and,or,xor,sub
        template <typename XLEN, typename OPERATOR>
        ActionGroup* integer_reg_reg_handler(atlas::AtlasState* state);

        // integer reg-reg 32-bit operations
        ActionGroup* addw_handler(atlas::AtlasState* state);
        ActionGroup* subw_handler(atlas::AtlasState* state);

        // addi,slti,sltui,andi,ori,xori
        template <typename XLEN, typename OPERATOR>
        ActionGroup* integer_reg_imm_handler(atlas::AtlasState* state);

        // integer reg-imm 32-bit operations
        ActionGroup* addiw_handler(atlas::AtlasState* state);

        // move and nop pseudo insts
        template <typename XLEN> ActionGroup* mv_handler(atlas::AtlasState* state);
        ActionGroup* nop_handler(atlas::AtlasState* state);

        // compute address for loads and stores
        template <typename XLEN, typename SIZE>
        ActionGroup* compute_address_handler(atlas::AtlasState* state);

        // lb,lbu,lh,lw,ld
        template <typename XLEN, typename SIZE, bool SIGN_EXTEND = false>
        ActionGroup* load_handler(atlas::AtlasState* state);

        // sb,sh,sw,sd
        template <typename XLEN, typename SIZE>
        ActionGroup* store_handler(atlas::AtlasState* state);

        // beq,bge,bgeu,blt,bltu,bne
        template <typename XLEN, typename OPERATOR>
        ActionGroup* branch_handler(atlas::AtlasState* state);

        // jumps
        template <typename XLEN> ActionGroup* jal_handler(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* jalr_handler(atlas::AtlasState* state);

        // load imm, load upper imm, add upper imm to pc
        template <typename XLEN> ActionGroup* li_handler(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* lui_handler(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* auipc_handler(atlas::AtlasState* state);

        // shifts,
        ActionGroup* sll_handler(atlas::AtlasState* state);
        ActionGroup* slli_handler(atlas::AtlasState* state);
        ActionGroup* slliw_handler(atlas::AtlasState* state);
        ActionGroup* sllw_handler(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* sra_handler(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* srai_handler(atlas::AtlasState* state);
        ActionGroup* sraiw_handler(atlas::AtlasState* state);
        ActionGroup* sraw_handler(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* srl_handler(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* srli_handler(atlas::AtlasState* state);
        ActionGroup* srliw_handler(atlas::AtlasState* state);
        ActionGroup* srlw_handler(atlas::AtlasState* state);

        // returns, environment calls, breakpoints, fences
        template <typename XLEN, PrivMode PRIV_MODE>
        ActionGroup* xret_handler(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* ecall_handler(atlas::AtlasState* state);
        ActionGroup* ebreak_handler(atlas::AtlasState* state);
        ActionGroup* fence_handler(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* sfence_vma_handler(atlas::AtlasState* state);

        // wfi
        template <typename XLEN> ActionGroup* wfi_handler(atlas::AtlasState* state);
    };
} // namespace atlas
