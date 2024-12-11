#pragma once

#include <map>
#include <string>

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
        ActionGroup* amoadd_d_64_compute_address_handler(atlas::AtlasState* state);
        ActionGroup* amoadd_d_64_handler(atlas::AtlasState* state);
        ActionGroup* amoadd_w_64_compute_address_handler(atlas::AtlasState* state);
        ActionGroup* amoadd_w_64_handler(atlas::AtlasState* state);
        ActionGroup* amoand_d_64_compute_address_handler(atlas::AtlasState* state);
        ActionGroup* amoand_d_64_handler(atlas::AtlasState* state);
        ActionGroup* amoand_w_64_compute_address_handler(atlas::AtlasState* state);
        ActionGroup* amoand_w_64_handler(atlas::AtlasState* state);
        ActionGroup* amomax_d_64_compute_address_handler(atlas::AtlasState* state);
        ActionGroup* amomax_d_64_handler(atlas::AtlasState* state);
        ActionGroup* amomax_w_64_compute_address_handler(atlas::AtlasState* state);
        ActionGroup* amomax_w_64_handler(atlas::AtlasState* state);
        ActionGroup* amomaxu_d_64_compute_address_handler(atlas::AtlasState* state);
        ActionGroup* amomaxu_d_64_handler(atlas::AtlasState* state);
        ActionGroup* amomaxu_w_64_compute_address_handler(atlas::AtlasState* state);
        ActionGroup* amomaxu_w_64_handler(atlas::AtlasState* state);
        ActionGroup* amomin_d_64_compute_address_handler(atlas::AtlasState* state);
        ActionGroup* amomin_d_64_handler(atlas::AtlasState* state);
        ActionGroup* amomin_w_64_compute_address_handler(atlas::AtlasState* state);
        ActionGroup* amomin_w_64_handler(atlas::AtlasState* state);
        ActionGroup* amominu_d_64_compute_address_handler(atlas::AtlasState* state);
        ActionGroup* amominu_d_64_handler(atlas::AtlasState* state);
        ActionGroup* amominu_w_64_compute_address_handler(atlas::AtlasState* state);
        ActionGroup* amominu_w_64_handler(atlas::AtlasState* state);
        ActionGroup* amoor_d_64_compute_address_handler(atlas::AtlasState* state);
        ActionGroup* amoor_d_64_handler(atlas::AtlasState* state);
        ActionGroup* amoor_w_64_compute_address_handler(atlas::AtlasState* state);
        ActionGroup* amoor_w_64_handler(atlas::AtlasState* state);
        ActionGroup* amoswap_d_64_compute_address_handler(atlas::AtlasState* state);
        ActionGroup* amoswap_d_64_handler(atlas::AtlasState* state);
        ActionGroup* amoswap_w_64_compute_address_handler(atlas::AtlasState* state);
        ActionGroup* amoswap_w_64_handler(atlas::AtlasState* state);
        ActionGroup* amoxor_d_64_compute_address_handler(atlas::AtlasState* state);
        ActionGroup* amoxor_d_64_handler(atlas::AtlasState* state);
        ActionGroup* amoxor_w_64_compute_address_handler(atlas::AtlasState* state);
        ActionGroup* amoxor_w_64_handler(atlas::AtlasState* state);
        ActionGroup* lr_d_64_compute_address_handler(atlas::AtlasState* state);
        ActionGroup* lr_d_64_handler(atlas::AtlasState* state);
        ActionGroup* lr_w_64_compute_address_handler(atlas::AtlasState* state);
        ActionGroup* lr_w_64_handler(atlas::AtlasState* state);
        ActionGroup* sc_d_64_compute_address_handler(atlas::AtlasState* state);
        ActionGroup* sc_d_64_handler(atlas::AtlasState* state);
        ActionGroup* sc_w_64_compute_address_handler(atlas::AtlasState* state);
        ActionGroup* sc_w_64_handler(atlas::AtlasState* state);
    };
} // namespace atlas
