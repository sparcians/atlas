#pragma once

#include <map>
#include <string>

namespace atlas
{
    class AtlasState;
    class Action;
    class ActionGroup;

    class RvdInsts
    {
      public:
        using base_type = RvdInsts;

        template <typename XLEN>
        static void getInstComputeAddressHandlers(std::map<std::string, Action> & inst_handlers);
        template <typename XLEN>
        static void getInstHandlers(std::map<std::string, Action> & inst_handlers);

      private:
        ActionGroup* fadd_d_64_handler(atlas::AtlasState* state);
        ActionGroup* fclass_d_64_handler(atlas::AtlasState* state);
        ActionGroup* fcvt_d_l_64_handler(atlas::AtlasState* state);
        ActionGroup* fcvt_d_lu_64_handler(atlas::AtlasState* state);
        ActionGroup* fcvt_d_s_64_handler(atlas::AtlasState* state);
        ActionGroup* fcvt_d_w_64_handler(atlas::AtlasState* state);
        ActionGroup* fcvt_d_wu_64_handler(atlas::AtlasState* state);
        ActionGroup* fcvt_l_d_64_handler(atlas::AtlasState* state);
        ActionGroup* fcvt_lu_d_64_handler(atlas::AtlasState* state);
        ActionGroup* fcvt_s_d_64_handler(atlas::AtlasState* state);
        ActionGroup* fcvt_w_d_64_handler(atlas::AtlasState* state);
        ActionGroup* fcvt_wu_d_64_handler(atlas::AtlasState* state);
        ActionGroup* fdiv_d_64_handler(atlas::AtlasState* state);
        ActionGroup* feq_d_64_handler(atlas::AtlasState* state);
        ActionGroup* fld_64_compute_address_handler(atlas::AtlasState* state);
        ActionGroup* fld_64_handler(atlas::AtlasState* state);
        ActionGroup* fle_d_64_handler(atlas::AtlasState* state);
        ActionGroup* flt_d_64_handler(atlas::AtlasState* state);
        ActionGroup* flw_64_compute_address_handler(atlas::AtlasState* state);
        ActionGroup* flw_64_handler(atlas::AtlasState* state);
        ActionGroup* fmadd_d_64_handler(atlas::AtlasState* state);
        ActionGroup* fmax_d_64_handler(atlas::AtlasState* state);
        ActionGroup* fmin_d_64_handler(atlas::AtlasState* state);
        ActionGroup* fmsub_d_64_handler(atlas::AtlasState* state);
        ActionGroup* fmul_d_64_handler(atlas::AtlasState* state);
        ActionGroup* fmv_d_x_64_handler(atlas::AtlasState* state);
        ActionGroup* fmv_w_x_64_handler(atlas::AtlasState* state);
        ActionGroup* fmv_x_d_64_handler(atlas::AtlasState* state);
        ActionGroup* fmv_x_w_64_handler(atlas::AtlasState* state);
        ActionGroup* fnmadd_d_64_handler(atlas::AtlasState* state);
        ActionGroup* fnmsub_d_64_handler(atlas::AtlasState* state);
        ActionGroup* fsd_64_compute_address_handler(atlas::AtlasState* state);
        ActionGroup* fsd_64_handler(atlas::AtlasState* state);
        ActionGroup* fsgnj_d_64_handler(atlas::AtlasState* state);
        ActionGroup* fsgnjn_d_64_handler(atlas::AtlasState* state);
        ActionGroup* fsgnjx_d_64_handler(atlas::AtlasState* state);
        ActionGroup* fsqrt_d_64_handler(atlas::AtlasState* state);
        ActionGroup* fsub_d_64_handler(atlas::AtlasState* state);
        ActionGroup* fsw_64_compute_address_handler(atlas::AtlasState* state);
        ActionGroup* fsw_64_handler(atlas::AtlasState* state);
    };
} // namespace atlas
