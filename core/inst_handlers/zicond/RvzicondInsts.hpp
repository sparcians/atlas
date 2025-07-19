#pragma once

#include "core/Execute.hpp"

namespace pegasus
{
    class PegasusState;

    class RvzicondInsts
    {
      public:
        using base_type = RvzicondInsts;

        template <typename XLEN> static void getInstHandlers(Execute::InstHandlersMap &);

      private:
        template <typename XLEN, typename OPERATOR>
        Action::ItrType czeroHandler_(pegasus::PegasusState* state, Action::ItrType action_it);
    };
} // namespace pegasus
