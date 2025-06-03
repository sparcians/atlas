#pragma once

#include "core/Action.hpp"

#include <map>
#include <string>
#include <stdint.h>

namespace atlas
{
    class AtlasState;

    class RvviaInsts
    {
      public:
        using base_type = RvviaInsts;

        template <typename XLEN>
        static void getInstHandlers(std::map<std::string, Action> & inst_handlers);

      private:
        template <template <typename> typename OP>
        Action::ItrType viavvHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        template <typename XLEN, template <typename> typename OP>
        Action::ItrType viavxHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        template <template <typename> typename OP>
        Action::ItrType viaviHandler_(atlas::AtlasState* state, Action::ItrType action_it);
    };
} // namespace atlas
