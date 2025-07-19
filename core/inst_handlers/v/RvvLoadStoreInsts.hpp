#pragma once

#include <map>
#include <string>
#include <stdint.h>

#include "core/Action.hpp"

namespace pegasus
{
    class PegasusState;
    class Action;
    class ActionGroup;

    class RvvLoadStoreInsts
    {
      public:
        using base_type = RvvLoadStoreInsts;

        enum struct AddressingMode
        {
            UNIT,
            IDX_UNORDERED,
            STRIDED,
            IDX_ORDERED
        };

        template <typename XLEN>
        static void getInstComputeAddressHandlers(std::map<std::string, Action> & inst_handlers);
        template <typename XLEN>
        static void getInstHandlers(std::map<std::string, Action> & inst_handlers);

      private:
        template <typename XLEN, size_t elemWidth, AddressingMode addrMode>
        Action::ItrType vlseComputeAddressHandler_(pegasus::PegasusState* state,
                                                   Action::ItrType action_it);
        template <typename XLEN, size_t elemWidth, AddressingMode addrMode>
        Action::ItrType vlseIdxComputeAddressHandler_(pegasus::PegasusState* state,
                                                      Action::ItrType action_it);
        template <size_t elemWidth, bool load>
        Action::ItrType vlseHandler_(pegasus::PegasusState* state, Action::ItrType action_it);
        template <bool load>
        Action::ItrType vlseIdxHandler_(pegasus::PegasusState* state, Action::ItrType action_it);
    };
} // namespace pegasus
