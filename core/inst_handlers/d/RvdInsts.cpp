#include "core/inst_handlers/d/RvdInsts.hpp"
#include "include/ActionTags.hpp"
#include "core/ActionGroup.hpp"

constexpr uint64_t dp_sign_mask = 1UL << 63;

namespace pegasus
{
    template <typename XLEN>
    void RvdInsts::getInstComputeAddressHandlers(std::map<std::string, Action> & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);
        inst_handlers.emplace(
            "fld",
            pegasus::Action::createAction<&RvdInsts::computeAddressHandler<XLEN>, RvfInstsBase>(
                nullptr, "fld", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "fsd",
            pegasus::Action::createAction<&RvdInsts::computeAddressHandler<XLEN>, RvfInstsBase>(
                nullptr, "fsd", ActionTags::COMPUTE_ADDR_TAG));
    }

    template <typename XLEN>
    void RvdInsts::getInstHandlers(std::map<std::string, Action> & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);
        inst_handlers.emplace(
            "fadd_d", pegasus::Action::createAction<&RvdInsts::fadd_dHandler_<XLEN>, RvdInsts>(
                          nullptr, "fadd_d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fclass_d", pegasus::Action::createAction<&RvdInsts::fclass_dHandler_<XLEN>, RvdInsts>(
                            nullptr, "fclass_d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fcvt_d_s", pegasus::Action::createAction<&RvdInsts::fcvt_d_sHandler_<XLEN>, RvdInsts>(
                            nullptr, "fcvt_d_s", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fcvt_d_w", pegasus::Action::createAction<&RvdInsts::fcvt_d_wHandler_<XLEN>, RvdInsts>(
                            nullptr, "fcvt_d_w", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fcvt_d_wu",
            pegasus::Action::createAction<&RvdInsts::fcvt_d_wuHandler_<XLEN>, RvdInsts>(
                nullptr, "fcvt_d_wu", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fcvt_s_d", pegasus::Action::createAction<&RvdInsts::fcvt_s_dHandler_<XLEN>, RvdInsts>(
                            nullptr, "fcvt_s_d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fcvt_w_d", pegasus::Action::createAction<&RvdInsts::fcvt_w_dHandler_<XLEN>, RvdInsts>(
                            nullptr, "fcvt_w_d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fcvt_wu_d",
            pegasus::Action::createAction<&RvdInsts::fcvt_wu_dHandler_<XLEN>, RvdInsts>(
                nullptr, "fcvt_wu_d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fdiv_d", pegasus::Action::createAction<&RvdInsts::fdiv_dHandler_<XLEN>, RvdInsts>(
                          nullptr, "fdiv_d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "feq_d", pegasus::Action::createAction<&RvdInsts::feq_dHandler_<XLEN>, RvdInsts>(
                         nullptr, "feq_d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fld", pegasus::Action::createAction<&RvdInsts::floatLsHandler<DP, true>, RvfInstsBase>(
                       nullptr, "fld", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fle_d", pegasus::Action::createAction<&RvdInsts::fle_dHandler_<XLEN>, RvdInsts>(
                         nullptr, "fle_d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "flt_d", pegasus::Action::createAction<&RvdInsts::flt_dHandler_<XLEN>, RvdInsts>(
                         nullptr, "flt_d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fmadd_d", pegasus::Action::createAction<&RvdInsts::fmadd_dHandler_<XLEN>, RvdInsts>(
                           nullptr, "fmadd_d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fmax_d", pegasus::Action::createAction<&RvdInsts::fmax_dHandler_<XLEN>, RvdInsts>(
                          nullptr, "fmax_d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fmin_d", pegasus::Action::createAction<&RvdInsts::fmin_dHandler_<XLEN>, RvdInsts>(
                          nullptr, "fmin_d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fmsub_d", pegasus::Action::createAction<&RvdInsts::fmsub_dHandler_<XLEN>, RvdInsts>(
                           nullptr, "fmsub_d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fmul_d", pegasus::Action::createAction<&RvdInsts::fmul_dHandler_<XLEN>, RvdInsts>(
                          nullptr, "fmul_d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fnmadd_d", pegasus::Action::createAction<&RvdInsts::fnmadd_dHandler_<XLEN>, RvdInsts>(
                            nullptr, "fnmadd_d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fnmsub_d", pegasus::Action::createAction<&RvdInsts::fnmsub_dHandler_<XLEN>, RvdInsts>(
                            nullptr, "fnmsub_d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fsd",
            pegasus::Action::createAction<&RvdInsts::floatLsHandler<DP, false>, RvfInstsBase>(
                nullptr, "fsd", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fsgnj_d", pegasus::Action::createAction<&RvdInsts::fsgnj_dHandler_<XLEN>, RvdInsts>(
                           nullptr, "fsgnj_d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fsgnjn_d", pegasus::Action::createAction<&RvdInsts::fsgnjn_dHandler_<XLEN>, RvdInsts>(
                            nullptr, "fsgnjn_d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fsgnjx_d", pegasus::Action::createAction<&RvdInsts::fsgnjx_dHandler_<XLEN>, RvdInsts>(
                            nullptr, "fsgnjx_d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fsqrt_d", pegasus::Action::createAction<&RvdInsts::fsqrt_dHandler_<XLEN>, RvdInsts>(
                           nullptr, "fsqrt_d", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fsub_d", pegasus::Action::createAction<&RvdInsts::fsub_dHandler_<XLEN>, RvdInsts>(
                          nullptr, "fsub_d", ActionTags::EXECUTE_TAG));
        if constexpr (sizeof(XLEN) >= sizeof(DP))
        {
            inst_handlers.emplace(
                "fcvt_d_l",
                pegasus::Action::createAction<&RvdInsts::fcvt_d_lHandler_<XLEN>, RvdInsts>(
                    nullptr, "fcvt_d_l", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "fcvt_d_lu",
                pegasus::Action::createAction<&RvdInsts::fcvt_d_luHandler_<XLEN>, RvdInsts>(
                    nullptr, "fcvt_d_lu", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "fcvt_l_d",
                pegasus::Action::createAction<&RvdInsts::fcvt_l_dHandler_<XLEN>, RvdInsts>(
                    nullptr, "fcvt_l_d", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "fcvt_lu_d",
                pegasus::Action::createAction<&RvdInsts::fcvt_lu_dHandler_<XLEN>, RvdInsts>(
                    nullptr, "fcvt_lu_d", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "fmv_d_x",
                pegasus::Action::createAction<&RvdInsts::fmv_d_xHandler_<XLEN>, RvdInsts>(
                    nullptr, "fmv_d_x", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "fmv_x_d",
                pegasus::Action::createAction<&RvdInsts::fmv_x_dHandler_<XLEN>, RvdInsts>(
                    nullptr, "fmv_x_d", ActionTags::EXECUTE_TAG));
        }
    }

    template void RvdInsts::getInstComputeAddressHandlers<RV32>(std::map<std::string, Action> &);
    template void RvdInsts::getInstComputeAddressHandlers<RV64>(std::map<std::string, Action> &);
    template void RvdInsts::getInstHandlers<RV32>(std::map<std::string, Action> &);
    template void RvdInsts::getInstHandlers<RV64>(std::map<std::string, Action> &);

    template <typename XLEN>
    Action::ItrType RvdInsts::fcvt_d_wHandler_(pegasus::PegasusState* state,
                                               Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        const uint32_t rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        WRITE_FP_REG<RV64>(state, inst->getRd(), i32_to_f64(rs1_val).v);
        updateCsr<XLEN>(state);
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvdInsts::fsub_dHandler_(pegasus::PegasusState* state,
                                             Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        softfloat_roundingMode = getRM<RV64>(state);
        const uint64_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        const uint64_t rs2_val = READ_FP_REG<RV64>(state, inst->getRs2());
        WRITE_FP_REG<RV64>(state, inst->getRd(), f64_sub(float64_t{rs1_val}, float64_t{rs2_val}).v);
        updateCsr<XLEN>(state);
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvdInsts::fmv_x_dHandler_(pegasus::PegasusState* state,
                                              Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        const uint64_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        WRITE_INT_REG<XLEN>(state, inst->getRd(), rs1_val);
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvdInsts::fcvt_wu_dHandler_(pegasus::PegasusState* state,
                                                Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        const uint64_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        WRITE_INT_REG<XLEN>(state, inst->getRd(),
                            signExtend<uint32_t, uint64_t>(
                                f64_to_ui32(float64_t{rs1_val}, getRM<RV64>(state), true)));
        updateCsr<XLEN>(state);
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvdInsts::fnmsub_dHandler_(pegasus::PegasusState* state,
                                               Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        softfloat_roundingMode = getRM<RV64>(state);
        const uint64_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        const uint64_t rs2_val = READ_FP_REG<RV64>(state, inst->getRs2());
        const uint64_t rs3_val = READ_FP_REG<RV64>(state, inst->getRs3());
        const uint64_t result =
            f64_mulAdd(float64_t{rs1_val ^ dp_sign_mask}, float64_t{rs2_val}, float64_t{rs3_val}).v;
        WRITE_FP_REG<RV64>(state, inst->getRd(), result);
        updateCsr<XLEN>(state);
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvdInsts::fle_dHandler_(pegasus::PegasusState* state, Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        const uint64_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        const uint64_t rs2_val = READ_FP_REG<RV64>(state, inst->getRs2());
        WRITE_INT_REG<XLEN>(state, inst->getRd(), f64_le(float64_t{rs1_val}, float64_t{rs2_val}));
        updateCsr<XLEN>(state);
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvdInsts::fmul_dHandler_(pegasus::PegasusState* state,
                                             Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        softfloat_roundingMode = getRM<RV64>(state);
        const uint64_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        const uint64_t rs2_val = READ_FP_REG<RV64>(state, inst->getRs2());
        WRITE_FP_REG<RV64>(state, inst->getRd(), f64_mul(float64_t{rs1_val}, float64_t{rs2_val}).v);
        updateCsr<XLEN>(state);
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvdInsts::fsqrt_dHandler_(pegasus::PegasusState* state,
                                              Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        softfloat_roundingMode = getRM<RV64>(state);
        const uint64_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        WRITE_FP_REG<RV64>(state, inst->getRd(), f64_sqrt(float64_t{rs1_val}).v);
        updateCsr<XLEN>(state);
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvdInsts::fmadd_dHandler_(pegasus::PegasusState* state,
                                              Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        softfloat_roundingMode = getRM<RV64>(state);
        const uint64_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        const uint64_t rs2_val = READ_FP_REG<RV64>(state, inst->getRs2());
        const uint64_t rs3_val = READ_FP_REG<RV64>(state, inst->getRs3());
        WRITE_FP_REG<RV64>(
            state, inst->getRd(),
            f64_mulAdd(float64_t{rs1_val}, float64_t{rs2_val}, float64_t{rs3_val}).v);
        updateCsr<XLEN>(state);
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvdInsts::fnmadd_dHandler_(pegasus::PegasusState* state,
                                               Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        softfloat_roundingMode = getRM<RV64>(state);
        const uint64_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        const uint64_t rs2_val = READ_FP_REG<RV64>(state, inst->getRs2());
        const uint64_t rs3_val = READ_FP_REG<RV64>(state, inst->getRs3());
        const uint64_t result = f64_mulAdd(float64_t{rs1_val ^ dp_sign_mask}, float64_t{rs2_val},
                                           float64_t{rs3_val ^ dp_sign_mask})
                                    .v;
        WRITE_FP_REG<RV64>(state, inst->getRd(), result);
        updateCsr<XLEN>(state);
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvdInsts::fmin_dHandler_(pegasus::PegasusState* state,
                                             Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        const uint64_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        const uint64_t rs2_val = READ_FP_REG<RV64>(state, inst->getRs2());
        uint64_t rd_val = f64_le_quiet(float64_t{rs1_val}, float64_t{rs2_val}) ? rs1_val : rs2_val;
        fmaxFminNanZeroCheck<DP>(rs1_val, rs2_val, rd_val, false);
        WRITE_FP_REG<RV64>(state, inst->getRd(), rd_val);
        updateCsr<XLEN>(state);
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvdInsts::fdiv_dHandler_(pegasus::PegasusState* state,
                                             Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        softfloat_roundingMode = getRM<RV64>(state);
        const uint64_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        const uint64_t rs2_val = READ_FP_REG<RV64>(state, inst->getRs2());
        WRITE_FP_REG<RV64>(state, inst->getRd(), f64_div(float64_t{rs1_val}, float64_t{rs2_val}).v);
        updateCsr<XLEN>(state);
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvdInsts::fsgnjx_dHandler_(pegasus::PegasusState* state,
                                               Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        const uint64_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        const uint64_t rs2_val = READ_FP_REG<RV64>(state, inst->getRs2());
        WRITE_FP_REG<RV64>(state, inst->getRd(),
                           (rs1_val & ~dp_sign_mask) | ((rs1_val ^ rs2_val) & dp_sign_mask));
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvdInsts::fmv_d_xHandler_(pegasus::PegasusState* state,
                                              Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        const uint64_t rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        WRITE_FP_REG<RV64>(state, inst->getRd(), rs1_val);
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvdInsts::fcvt_w_dHandler_(pegasus::PegasusState* state,
                                               Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        const uint64_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        WRITE_INT_REG<XLEN>(state, inst->getRd(),
                            signExtend<uint32_t, uint64_t>(
                                f64_to_i32(float64_t{rs1_val}, getRM<RV64>(state), true)));
        updateCsr<XLEN>(state);
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvdInsts::fcvt_lu_dHandler_(pegasus::PegasusState* state,
                                                Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        const uint64_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        WRITE_INT_REG<XLEN>(state, inst->getRd(),
                            f64_to_ui64(float64_t{rs1_val}, getRM<RV64>(state), true));
        updateCsr<XLEN>(state);
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvdInsts::fsgnjn_dHandler_(pegasus::PegasusState* state,
                                               Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        const uint64_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        const uint64_t rs2_val = READ_FP_REG<RV64>(state, inst->getRs2());
        WRITE_FP_REG<RV64>(state, inst->getRd(),
                           (rs1_val & ~dp_sign_mask) | ((rs2_val & dp_sign_mask) ^ dp_sign_mask));
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvdInsts::fclass_dHandler_(pegasus::PegasusState* state,
                                               Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        softfloat_roundingMode = getRM<XLEN>(state);
        const uint64_t rs1_val = checkNanBoxing<RV64, DP>(READ_FP_REG<RV64>(state, inst->getRs1()));

        WRITE_INT_REG<XLEN>(state, inst->getRd(), fclass(rs1_val));

        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvdInsts::fadd_dHandler_(pegasus::PegasusState* state,
                                             Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        softfloat_roundingMode = getRM<RV64>(state);
        const uint64_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        const uint64_t rs2_val = READ_FP_REG<RV64>(state, inst->getRs2());
        WRITE_FP_REG<RV64>(state, inst->getRd(), f64_add(float64_t{rs1_val}, float64_t{rs2_val}).v);
        updateCsr<XLEN>(state);
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvdInsts::fmsub_dHandler_(pegasus::PegasusState* state,
                                              Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        softfloat_roundingMode = getRM<RV64>(state);
        const uint64_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        const uint64_t rs2_val = READ_FP_REG<RV64>(state, inst->getRs2());
        const uint64_t rs3_val = READ_FP_REG<RV64>(state, inst->getRs3());
        WRITE_FP_REG<RV64>(
            state, inst->getRd(),
            f64_mulAdd(float64_t{rs1_val}, float64_t{rs2_val}, float64_t{rs3_val ^ dp_sign_mask})
                .v);
        updateCsr<XLEN>(state);
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvdInsts::fcvt_d_sHandler_(pegasus::PegasusState* state,
                                               Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        const uint32_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        WRITE_FP_REG<RV64>(state, inst->getRd(), f32_to_f64(float32_t{rs1_val}).v);
        updateCsr<XLEN>(state);
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvdInsts::fmax_dHandler_(pegasus::PegasusState* state,
                                             Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        const uint64_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        const uint64_t rs2_val = READ_FP_REG<RV64>(state, inst->getRs2());
        uint64_t rd_val = f64_le_quiet(float64_t{rs1_val}, float64_t{rs2_val}) ? rs2_val : rs1_val;
        fmaxFminNanZeroCheck<DP>(rs1_val, rs2_val, rd_val, true);
        WRITE_FP_REG<RV64>(state, inst->getRd(), rd_val);
        updateCsr<XLEN>(state);
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvdInsts::fcvt_s_dHandler_(pegasus::PegasusState* state,
                                               Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        const uint64_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        softfloat_roundingMode = getRM<RV64>(state);
        WRITE_FP_REG<RV64>(state, inst->getRd(),
                           nanBoxing<RV64, SP>(f64_to_f32(float64_t{rs1_val}).v));
        updateCsr<XLEN>(state);
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvdInsts::fcvt_d_luHandler_(pegasus::PegasusState* state,
                                                Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        const uint64_t rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        WRITE_FP_REG<RV64>(state, inst->getRd(), ui64_to_f64(rs1_val).v);
        updateCsr<XLEN>(state);
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvdInsts::feq_dHandler_(pegasus::PegasusState* state, Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        const uint64_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        const uint64_t rs2_val = READ_FP_REG<RV64>(state, inst->getRs2());
        WRITE_INT_REG<XLEN>(state, inst->getRd(), f64_eq(float64_t{rs1_val}, float64_t{rs2_val}));
        updateCsr<XLEN>(state);
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvdInsts::fsgnj_dHandler_(pegasus::PegasusState* state,
                                              Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        const uint64_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        const uint64_t rs2_val = READ_FP_REG<RV64>(state, inst->getRs2());
        WRITE_FP_REG<RV64>(state, inst->getRd(),
                           (rs1_val & ~dp_sign_mask) | (rs2_val & dp_sign_mask));
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvdInsts::fcvt_d_lHandler_(pegasus::PegasusState* state,
                                               Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        const uint64_t rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        WRITE_FP_REG<RV64>(state, inst->getRd(), i64_to_f64(rs1_val).v);
        updateCsr<XLEN>(state);
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvdInsts::fcvt_d_wuHandler_(pegasus::PegasusState* state,
                                                Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        const uint32_t rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        WRITE_FP_REG<RV64>(state, inst->getRd(), ui32_to_f64(rs1_val).v);
        updateCsr<XLEN>(state);
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvdInsts::flt_dHandler_(pegasus::PegasusState* state, Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        const uint64_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        const uint64_t rs2_val = READ_FP_REG<RV64>(state, inst->getRs2());
        WRITE_INT_REG<XLEN>(state, inst->getRd(), f64_lt(float64_t{rs1_val}, float64_t{rs2_val}));
        updateCsr<XLEN>(state);
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvdInsts::fcvt_l_dHandler_(pegasus::PegasusState* state,
                                               Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        const uint64_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        WRITE_INT_REG<XLEN>(state, inst->getRd(),
                            f64_to_i64(float64_t{rs1_val}, getRM<RV64>(state), true));
        updateCsr<XLEN>(state);
        return ++action_it;
    }

} // namespace pegasus
