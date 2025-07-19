#pragma once

#include "include/PegasusTypes.hpp"
#include "core/Action.hpp"

#include <map>
#include <string>

namespace pegasus
{
    class PegasusState;

    class RviInsts
    {
      public:
        using base_type = RviInsts;

        template <typename XLEN>
        static void getInstComputeAddressHandlers(std::map<std::string, Action> &);
        template <typename XLEN> static void getInstHandlers(std::map<std::string, Action> &, bool);

      private:
        // add,slt,sltu,and,or,xor,sub
        template <typename XLEN, typename OPERATOR>
        Action::ItrType integer_reg_regHandler_(pegasus::PegasusState* state,
                                                Action::ItrType action_it);

        // integer reg-reg 32-bit operations
        Action::ItrType addwHandler_(pegasus::PegasusState* state, Action::ItrType action_it);
        Action::ItrType subwHandler_(pegasus::PegasusState* state, Action::ItrType action_it);

        // addi,slti,sltui,andi,ori,xori
        template <typename XLEN, typename OPERATOR>
        Action::ItrType integer_reg_immHandler_(pegasus::PegasusState* state,
                                                Action::ItrType action_it);

        // integer reg-imm 32-bit operations
        Action::ItrType addiwHandler_(pegasus::PegasusState* state, Action::ItrType action_it);

        // move and nop pseudo insts
        template <typename XLEN>
        Action::ItrType mvHandler_(pegasus::PegasusState* state, Action::ItrType action_it);
        Action::ItrType nopHandler_(pegasus::PegasusState* state, Action::ItrType action_it);

        // compute address for loads and stores
        template <typename XLEN, typename SIZE>
        Action::ItrType computeAddressHandler_(pegasus::PegasusState* state,
                                               Action::ItrType action_it);

        // lb,lbu,lh,lw,ld
        template <typename XLEN, typename SIZE, bool SIGN_EXTEND = false>
        Action::ItrType loadHandler_(pegasus::PegasusState* state, Action::ItrType action_it);

        // sb,sh,sw,sd
        template <typename XLEN, typename SIZE>
        Action::ItrType storeHandler_(pegasus::PegasusState* state, Action::ItrType action_it);

        // beq,bge,bgeu,blt,bltu,bne
        template <typename XLEN, typename OPERATOR>
        Action::ItrType branchHandler_(pegasus::PegasusState* state, Action::ItrType action_it);

        // jumps
        template <typename XLEN>
        Action::ItrType jalHandler_(pegasus::PegasusState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType jalrHandler_(pegasus::PegasusState* state, Action::ItrType action_it);

        // load imm, load upper imm, add upper imm to pc
        template <typename XLEN>
        Action::ItrType liHandler_(pegasus::PegasusState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType luiHandler_(pegasus::PegasusState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType auipcHandler_(pegasus::PegasusState* state, Action::ItrType action_it);

        // shifts,
        Action::ItrType sllHandler_(pegasus::PegasusState* state, Action::ItrType action_it);
        Action::ItrType slliHandler_(pegasus::PegasusState* state, Action::ItrType action_it);
        Action::ItrType slliwHandler_(pegasus::PegasusState* state, Action::ItrType action_it);
        Action::ItrType sllwHandler_(pegasus::PegasusState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType sraHandler_(pegasus::PegasusState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType sraiHandler_(pegasus::PegasusState* state, Action::ItrType action_it);
        Action::ItrType sraiwHandler_(pegasus::PegasusState* state, Action::ItrType action_it);
        Action::ItrType srawHandler_(pegasus::PegasusState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType srlHandler_(pegasus::PegasusState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType srliHandler_(pegasus::PegasusState* state, Action::ItrType action_it);
        Action::ItrType srliwHandler_(pegasus::PegasusState* state, Action::ItrType action_it);
        Action::ItrType srlwHandler_(pegasus::PegasusState* state, Action::ItrType action_it);

        // returns, environment calls, breakpoints, fences
        template <typename XLEN, PrivMode PRIV_MODE>
        Action::ItrType xretHandler_(pegasus::PegasusState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType ecallHandler_(pegasus::PegasusState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType ecallHandlerSystemEmulation_(pegasus::PegasusState* state,
                                                     Action::ItrType action_it);
        Action::ItrType ebreakHandler_(pegasus::PegasusState* state, Action::ItrType action_it);
        Action::ItrType fenceHandler_(pegasus::PegasusState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType sfence_vmaHandler_(pegasus::PegasusState* state, Action::ItrType action_it);

        // wfi
        template <typename XLEN>
        Action::ItrType wfiHandler_(pegasus::PegasusState* state, Action::ItrType action_it);
    };
} // namespace pegasus
