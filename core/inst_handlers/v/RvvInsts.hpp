#pragma once

#include <map>
#include <string>
#include <stdint.h>

namespace atlas
{
    class AtlasState;
    class Action;
    class ActionGroup;

    class RvvInsts
    {
      public:
        using base_type = RvvInsts;

        template <typename XLEN>
        static void getInstComputeAddressHandlers(std::map<std::string, Action> & inst_handlers);
        template <typename XLEN>
        static void getInstHandlers(std::map<std::string, Action> & inst_handlers);

      private:
        template <typename XLEN, typename VLEN>
        ActionGroup* vsetvl_handler(atlas::AtlasState* state);
        template <typename XLEN, typename VLEN>
        ActionGroup* vsetvli_handler(atlas::AtlasState* state);
        template <typename XLEN, typename VLEN>
        ActionGroup* vsetivli_handler(atlas::AtlasState* state);
    };
} // namespace atlas
