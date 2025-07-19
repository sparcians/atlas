#pragma once

#include <map>
#include <string>
#include <stdint.h>

#include "core/Action.hpp"

namespace pegasus
{
    class PegasusState;

    class RvvConfigSettingInsts
    {
      public:
        using base_type = RvvConfigSettingInsts;

        template <typename XLEN>
        static void getInstHandlers(std::map<std::string, Action> & inst_handlers);

      private:
        template <typename XLEN>
        Action::ItrType vsetvlHandler_(pegasus::PegasusState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType vsetvliHandler_(pegasus::PegasusState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType vsetivliHandler_(pegasus::PegasusState* state, Action::ItrType action_it);
    };
} // namespace pegasus
