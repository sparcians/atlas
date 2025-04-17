#pragma once

#include <map>
#include <string>
#include <stdint.h>

namespace atlas
{
    class AtlasState;
    class Action;
    class ActionGroup;

    class RvaInsts
    {
      public:
        using base_type = RvaInsts;

        template <typename XLEN>
        static void getInstComputeAddressHandlers(std::map<std::string, Action> & inst_handlers);
        template <typename XLEN>
        static void getInstHandlers(std::map<std::string, Action> & inst_handlers);

      private:
        template <typename T> ActionGroup* compute_address_handler(AtlasState* state);

        template <typename RV, typename SIZE, typename OP, bool U = true>
        ActionGroup* amo_handler(atlas::AtlasState* state);

        ActionGroup* lr_d_64_handler(atlas::AtlasState* state);
        ActionGroup* lr_w_64_handler(atlas::AtlasState* state);
        ActionGroup* sc_d_64_handler(atlas::AtlasState* state);
        ActionGroup* sc_w_64_handler(atlas::AtlasState* state);
    };
} // namespace atlas
