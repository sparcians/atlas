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
        ActionGroup* addw_64_handler(atlas::AtlasState* state);
        ActionGroup* subw_64_handler(atlas::AtlasState* state);

        // addi,slti,sltui,andi,ori,xori
        template <typename XLEN, typename OPERATOR>
        ActionGroup* integer_reg_imm_handler(atlas::AtlasState* state);

        // integer reg-imm 32-bit operations
        ActionGroup* addiw_64_handler(atlas::AtlasState* state);

        // move and nop pseudo insts
        ActionGroup* mv_64_handler(atlas::AtlasState* state);
        ActionGroup* nop_64_handler(atlas::AtlasState* state);

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
        ActionGroup* jal_64_handler(atlas::AtlasState* state);
        ActionGroup* jalr_64_handler(atlas::AtlasState* state);

        // load imm, load upper imm, add upper imm to pc
        ActionGroup* li_64_handler(atlas::AtlasState* state);
        ActionGroup* lui_64_handler(atlas::AtlasState* state);
        ActionGroup* auipc_64_handler(atlas::AtlasState* state);

        // shifts,
        ActionGroup* sll_64_handler(atlas::AtlasState* state);
        ActionGroup* slli_64_handler(atlas::AtlasState* state);
        ActionGroup* slliw_64_handler(atlas::AtlasState* state);
        ActionGroup* sllw_64_handler(atlas::AtlasState* state);
        ActionGroup* sra_64_handler(atlas::AtlasState* state);
        ActionGroup* srai_64_handler(atlas::AtlasState* state);
        ActionGroup* sraiw_64_handler(atlas::AtlasState* state);
        ActionGroup* sraw_64_handler(atlas::AtlasState* state);
        ActionGroup* srl_64_handler(atlas::AtlasState* state);
        ActionGroup* srli_64_handler(atlas::AtlasState* state);
        ActionGroup* srliw_64_handler(atlas::AtlasState* state);
        ActionGroup* srlw_64_handler(atlas::AtlasState* state);

        // returns, environment calls, breakpoints, fences
        template <typename XLEN, PrivMode PRIV_MODE>
        ActionGroup* xret_handler(atlas::AtlasState* state);
        ActionGroup* ecall_64_handler(atlas::AtlasState* state);
        ActionGroup* ebreak_64_handler(atlas::AtlasState* state);
        ActionGroup* fence_64_handler(atlas::AtlasState* state);
        ActionGroup* sfence_vma_64_handler(atlas::AtlasState* state);

        // wfi
        ActionGroup* wfi_64_handler(atlas::AtlasState* state);
    };
} // namespace atlas
