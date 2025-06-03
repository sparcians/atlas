#pragma once

#include "core/Action.hpp"

#include <map>
#include <string>
#include <stdint.h>

namespace atlas
{
    class AtlasState;
    class Action;
    class ActionGroup;

    class RvvlsInsts
    {
      public:
        using base_type = RvvlsInsts;

        enum struct AddressingMode
        {
            Unit,
            IdxUnordered,
            Strided,
            IdxOrdered,
        };

        template <typename XLEN>
        static void getInstComputeAddressHandlers(std::map<std::string, Action> & inst_handlers);
        template <typename XLEN>
        static void getInstHandlers(std::map<std::string, Action> & inst_handlers);

      private:
        template <typename XLEN, uint8_t ElemWidth, AddressingMode Mode>
        Action::ItrType vlseComputeAddressHandler_(atlas::AtlasState* state,
                                                   Action::ItrType action_it);
        template <typename XLEN, uint8_t ElemWidth, AddressingMode Mode>
        Action::ItrType vlseIdxComputeAddressHandler_(atlas::AtlasState* state,
                                                      Action::ItrType action_it);
        template <uint8_t ElemWidth, bool load>
        Action::ItrType vlseHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        template <bool load>
        Action::ItrType vlseIdxHandler_(atlas::AtlasState* state, Action::ItrType action_it);
    };
} // namespace atlas
