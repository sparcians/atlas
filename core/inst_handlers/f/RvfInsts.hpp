#pragma once

#include <map>
#include <string>

namespace atlas
{
    class AtlasState;
    class Action;
    class ActionGroup;

    class RvfInsts
    {
      public:
        using base_type = RvfInsts;

        template <typename XLEN>
        static void getInstComputeAddressHandlers(std::map<std::string, Action> & inst_handlers);
        template <typename XLEN>
        static void getInstHandlers(std::map<std::string, Action> & inst_handlers);

      private:
        ActionGroup* fadd_s_64_handler(atlas::AtlasState* state);
        ActionGroup* fclass_s_64_handler(atlas::AtlasState* state);
        ActionGroup* fcvt_l_s_64_handler(atlas::AtlasState* state);
        ActionGroup* fcvt_lu_s_64_handler(atlas::AtlasState* state);
        ActionGroup* fcvt_s_l_64_handler(atlas::AtlasState* state);
        ActionGroup* fcvt_s_lu_64_handler(atlas::AtlasState* state);
        ActionGroup* fcvt_s_w_64_handler(atlas::AtlasState* state);
        ActionGroup* fcvt_s_wu_64_handler(atlas::AtlasState* state);
        ActionGroup* fcvt_w_s_64_handler(atlas::AtlasState* state);
        ActionGroup* fcvt_wu_s_64_handler(atlas::AtlasState* state);
        ActionGroup* fdiv_s_64_handler(atlas::AtlasState* state);
        ActionGroup* feq_s_64_handler(atlas::AtlasState* state);
        ActionGroup* fle_s_64_handler(atlas::AtlasState* state);
        ActionGroup* flt_s_64_handler(atlas::AtlasState* state);
        ActionGroup* flw_64_compute_address_handler(atlas::AtlasState* state);
        ActionGroup* flw_64_handler(atlas::AtlasState* state);
        ActionGroup* fmadd_s_64_handler(atlas::AtlasState* state);
        ActionGroup* fmax_s_64_handler(atlas::AtlasState* state);
        ActionGroup* fmin_s_64_handler(atlas::AtlasState* state);
        ActionGroup* fmsub_s_64_handler(atlas::AtlasState* state);
        ActionGroup* fmul_s_64_handler(atlas::AtlasState* state);
        ActionGroup* fmv_w_x_64_handler(atlas::AtlasState* state);
        ActionGroup* fmv_x_w_64_handler(atlas::AtlasState* state);
        ActionGroup* fnmadd_s_64_handler(atlas::AtlasState* state);
        ActionGroup* fnmsub_s_64_handler(atlas::AtlasState* state);
        ActionGroup* fsgnj_s_64_handler(atlas::AtlasState* state);
        ActionGroup* fsgnjn_s_64_handler(atlas::AtlasState* state);
        ActionGroup* fsgnjx_s_64_handler(atlas::AtlasState* state);
        ActionGroup* fsqrt_s_64_handler(atlas::AtlasState* state);
        ActionGroup* fsub_s_64_handler(atlas::AtlasState* state);
        ActionGroup* fsw_64_compute_address_handler(atlas::AtlasState* state);
        ActionGroup* fsw_64_handler(atlas::AtlasState* state);
    };
} // namespace atlas
