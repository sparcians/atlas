#pragma once

#include "core/Execute.hpp"

namespace pegasus
{
    class PegasusState;

    class RvmInsts
    {
      public:
        using base_type = RvmInsts;

        template <typename XLEN> static void getInstHandlers(Execute::InstHandlersMap &);

      private:
        template <typename XLEN>
        Action::ItrType div_64Handler_(pegasus::PegasusState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType divu_64Handler_(pegasus::PegasusState* state, Action::ItrType action_it);
        Action::ItrType divuw_64Handler_(pegasus::PegasusState* state, Action::ItrType action_it);
        Action::ItrType divw_64Handler_(pegasus::PegasusState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType mul_64Handler_(pegasus::PegasusState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType mulh_64Handler_(pegasus::PegasusState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType mulhsu_64Handler_(pegasus::PegasusState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType mulhu_64Handler_(pegasus::PegasusState* state, Action::ItrType action_it);
        Action::ItrType mulw_64Handler_(pegasus::PegasusState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType rem_64Handler_(pegasus::PegasusState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType remu_64Handler_(pegasus::PegasusState* state, Action::ItrType action_it);
        Action::ItrType remuw_64Handler_(pegasus::PegasusState* state, Action::ItrType action_it);
        Action::ItrType remw_64Handler_(pegasus::PegasusState* state, Action::ItrType action_it);
    };
} // namespace pegasus
