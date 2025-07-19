#pragma once

#include "include/PegasusTypes.hpp"
#include "core/Action.hpp"

#include <map>
#include <string>

namespace pegasus
{
    class PegasusState;

    class RvzbcInsts
    {
      public:
        using base_type = RvzbcInsts;

        template <typename XLEN> static void getInstHandlers(std::map<std::string, Action> &);

      private:
        template <typename XLEN>
        Action::ItrType clmulHandler(pegasus::PegasusState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType clmulhHandler(pegasus::PegasusState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType clmulrHandler(pegasus::PegasusState* state, Action::ItrType action_it);
    };
} // namespace pegasus