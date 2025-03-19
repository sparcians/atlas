#pragma once

#include <map>
#include <string>
#include "core/inst_handlers/f/RvfInstsBase.hpp"

namespace atlas
{
    class AtlasState;
    class Action;
    class ActionGroup;

    class RvfInsts : RvfInstsBase
    {
      public:
        using base_type = RvfInsts;

        template <typename XLEN>
        static void getInstComputeAddressHandlers(std::map<std::string, Action> & inst_handlers);
        template <typename XLEN>
        static void getInstHandlers(std::map<std::string, Action> & inst_handlers);

      private:
        template <typename XLEN> ActionGroup* fadd_s_handler(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fclass_s_handler(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fcvt_l_s_handler(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fcvt_lu_s_handler(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fcvt_s_l_handler(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fcvt_s_lu_handler(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fcvt_s_w_handler(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fcvt_s_wu_handler(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fcvt_w_s_handler(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fcvt_wu_s_handler(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fdiv_s_handler(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* feq_s_handler(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fle_s_handler(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* flt_s_handler(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fmadd_s_handler(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fmax_s_handler(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fmin_s_handler(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fmsub_s_handler(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fmul_s_handler(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fmv_w_x_handler(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fmv_x_w_handler(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fnmadd_s_handler(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fnmsub_s_handler(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fsgnj_s_handler(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fsgnjn_s_handler(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fsgnjx_s_handler(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fsqrt_s_handler(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* fsub_s_handler(atlas::AtlasState* state);
        template <typename XLEN> ActionGroup* computeAddressHandler(atlas::AtlasState* state);
        template <typename SIZE, bool LOAD> ActionGroup* floatLsHandler(atlas::AtlasState* state);
    };
} // namespace atlas
