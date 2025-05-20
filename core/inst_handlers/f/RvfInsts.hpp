#pragma once

#include <map>
#include <string>
#include "core/inst_handlers/f/RvfInstsBase.hpp"

namespace atlas
{
    class AtlasState;
    class Action;
    class ActionGroup;

    class RvfInsts : public RvfInstsBase
    {
      public:
        template <typename XLEN>
        static void getInstComputeAddressHandlers(std::map<std::string, Action> & inst_handlers);
        template <typename XLEN>
        static void getInstHandlers(std::map<std::string, Action> & inst_handlers);

      private:
        template <typename XLEN>
        Action::ItrType fadd_sHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType fclass_sHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType fcvt_l_sHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType fcvt_lu_sHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType fcvt_s_lHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType fcvt_s_luHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType fcvt_s_wHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType fcvt_s_wuHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType fcvt_w_sHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType fcvt_wu_sHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType fdiv_sHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType feq_sHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType fle_sHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType flt_sHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType fmadd_sHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType fmax_sHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType fmin_sHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType fmsub_sHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType fmul_sHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType fmv_w_xHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType fmv_x_wHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType fnmadd_sHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType fnmsub_sHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType fsgnj_sHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType fsgnjn_sHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType fsgnjx_sHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType fsqrt_sHandler_(atlas::AtlasState* state, Action::ItrType action_it);
        template <typename XLEN>
        Action::ItrType fsub_sHandler_(atlas::AtlasState* state, Action::ItrType action_it);
    };
} // namespace atlas
