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
        template <typename VLEN, template <typename> typename OP>
        Action::ItrType viavv_handler_(atlas::AtlasState* state, Action::ItrType action_it);
        template <typename XLEN, typename VLEN, template <typename> typename OP>
        Action::ItrType viavx_handler_(atlas::AtlasState* state, Action::ItrType action_it);
        template <typename VLEN, template <typename> typename OP>
        Action::ItrType viavi_handler_(atlas::AtlasState* state, Action::ItrType action_it);
    };
} // namespace atlas
