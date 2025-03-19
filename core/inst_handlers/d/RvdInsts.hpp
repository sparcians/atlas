#pragma once

#include <map>
#include <string>
#include "core/inst_handlers/f/RvfInstsBase.hpp"

namespace atlas
{
    class AtlasState;
    class Action;
    class ActionGroup;

    class RvdInsts : RvfInstsBase
    {
      public:
        using base_type = RvdInsts;

        template <typename XLEN>
        static void getInstComputeAddressHandlers(std::map<std::string, Action> & inst_handlers);
        template <typename XLEN>
        static void getInstHandlers(std::map<std::string, Action> & inst_handlers);

      private:
        template <typename XLEN> ActionGroup* fadd_d_handler(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fclass_d_handler(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fcvt_d_l_handler(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fcvt_d_lu_handler(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fcvt_d_s_handler(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fcvt_d_w_handler(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fcvt_d_wu_handler(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fcvt_l_d_handler(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fcvt_lu_d_handler(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fcvt_s_d_handler(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fcvt_w_d_handler(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fcvt_wu_d_handler(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fdiv_d_handler(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* feq_d_handler(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fle_d_handler(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* flt_d_handler(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fmadd_d_handler(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fmax_d_handler(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fmin_d_handler(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fmsub_d_handler(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fmul_d_handler(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fmv_d_x_handler(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fmv_w_x_handler(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fmv_x_d_handler(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fmv_x_w_handler(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fnmadd_d_handler(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fnmsub_d_handler(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fsgnj_d_handler(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fsgnjn_d_handler(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fsgnjx_d_handler(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fsqrt_d_handler(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fsub_d_handler(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* computeAddressHandler(atlas::AtlasState* state);
        template <typename SIZE, bool LOAD> ActionGroup* floatLsHandler(atlas::AtlasState* state);
    };
} // namespace atlas
