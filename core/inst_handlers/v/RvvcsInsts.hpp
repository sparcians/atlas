#pragma once

#include "core/Action.hpp"

#include <map>
#include <string>
#include <stdint.h>

namespace atlas
{
    class AtlasState;

    class RvvcsInsts
    {
      public:
        using base_type = RvvcsInsts;

        template <typename XLEN>
        static void getInstHandlers(std::map<std::string, Action> & inst_handlers);

      private:
        template <typename XLEN>
        Action::ItrType vsetvlHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType vsetvliHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType vsetivliHandler_(atlas::AtlasState* state, Action::ItrType action_it);
    };
} // namespace atlas
