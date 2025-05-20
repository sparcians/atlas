#include "core/inst_handlers/f/RvfInsts.hpp"
#include "include/ActionTags.hpp"
#include "core/ActionGroup.hpp"
#include "core/AtlasState.hpp"
#include "core/AtlasInst.hpp"

namespace atlas
{
    template <typename XLEN>
    void RvfInsts::getInstComputeAddressHandlers(std::map<std::string, Action> & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);
        inst_handlers.emplace(
            "flw",
            atlas::Action::createAction<&RvfInsts::computeAddressHandler<XLEN>, RvfInstsBase>(
                nullptr, "flw", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "fsw",
            atlas::Action::createAction<&RvfInsts::computeAddressHandler<XLEN>, RvfInstsBase>(
                nullptr, "fsw", ActionTags::COMPUTE_ADDR_TAG));
    }

    template <typename XLEN>
    void RvfInsts::getInstHandlers(std::map<std::string, Action> & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);
        inst_handlers.emplace(
            "fadd_s", atlas::Action::createAction<&RvfInsts::fadd_sHandler_<XLEN>, RvfInsts>(
                          nullptr, "fadd_s", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fclass_s", atlas::Action::createAction<&RvfInsts::fclass_sHandler_<XLEN>, RvfInsts>(
                            nullptr, "fclass_s", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fcvt_l_s", atlas::Action::createAction<&RvfInsts::fcvt_l_sHandler_<XLEN>, RvfInsts>(
                            nullptr, "fcvt_l_s", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fcvt_lu_s", atlas::Action::createAction<&RvfInsts::fcvt_lu_sHandler_<XLEN>, RvfInsts>(
                             nullptr, "fcvt_lu_s", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fcvt_s_l", atlas::Action::createAction<&RvfInsts::fcvt_s_lHandler_<XLEN>, RvfInsts>(
                            nullptr, "fcvt_s_l", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fcvt_s_lu", atlas::Action::createAction<&RvfInsts::fcvt_s_luHandler_<XLEN>, RvfInsts>(
                             nullptr, "fcvt_s_lu", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fcvt_s_w", atlas::Action::createAction<&RvfInsts::fcvt_s_wHandler_<XLEN>, RvfInsts>(
                            nullptr, "fcvt_s_w", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fcvt_s_wu", atlas::Action::createAction<&RvfInsts::fcvt_s_wuHandler_<XLEN>, RvfInsts>(
                             nullptr, "fcvt_s_wu", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fcvt_w_s", atlas::Action::createAction<&RvfInsts::fcvt_w_sHandler_<XLEN>, RvfInsts>(
                            nullptr, "fcvt_w_s", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fcvt_wu_s", atlas::Action::createAction<&RvfInsts::fcvt_wu_sHandler_<XLEN>, RvfInsts>(
                             nullptr, "fcvt_wu_s", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fdiv_s", atlas::Action::createAction<&RvfInsts::fdiv_sHandler_<XLEN>, RvfInsts>(
                          nullptr, "fdiv_s", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("feq_s",
                              atlas::Action::createAction<&RvfInsts::feq_sHandler_<XLEN>, RvfInsts>(
                                  nullptr, "feq_s", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("fle_s",
                              atlas::Action::createAction<&RvfInsts::fle_sHandler_<XLEN>, RvfInsts>(
                                  nullptr, "fle_s", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("flt_s",
                              atlas::Action::createAction<&RvfInsts::flt_sHandler_<XLEN>, RvfInsts>(
                                  nullptr, "flt_s", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "flw", atlas::Action::createAction<&RvfInsts::floatLsHandler<SP, true>, RvfInstsBase>(
                       nullptr, "flw", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fmadd_s", atlas::Action::createAction<&RvfInsts::fmadd_sHandler_<XLEN>, RvfInsts>(
                           nullptr, "fmadd_s", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fmax_s", atlas::Action::createAction<&RvfInsts::fmax_sHandler_<XLEN>, RvfInsts>(
                          nullptr, "fmax_s", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fmin_s", atlas::Action::createAction<&RvfInsts::fmin_sHandler_<XLEN>, RvfInsts>(
                          nullptr, "fmin_s", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fmsub_s", atlas::Action::createAction<&RvfInsts::fmsub_sHandler_<XLEN>, RvfInsts>(
                           nullptr, "fmsub_s", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fmul_s", atlas::Action::createAction<&RvfInsts::fmul_sHandler_<XLEN>, RvfInsts>(
                          nullptr, "fmul_s", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fmv_w_x", atlas::Action::createAction<&RvfInsts::fmv_w_xHandler_<XLEN>, RvfInsts>(
                           nullptr, "fmv_w_x", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fmv_x_w", atlas::Action::createAction<&RvfInsts::fmv_x_wHandler_<XLEN>, RvfInsts>(
                           nullptr, "fmv_x_w", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fnmadd_s", atlas::Action::createAction<&RvfInsts::fnmadd_sHandler_<XLEN>, RvfInsts>(
                            nullptr, "fnmadd_s", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fnmsub_s", atlas::Action::createAction<&RvfInsts::fnmsub_sHandler_<XLEN>, RvfInsts>(
                            nullptr, "fnmsub_s", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fsgnj_s", atlas::Action::createAction<&RvfInsts::fsgnj_sHandler_<XLEN>, RvfInsts>(
                           nullptr, "fsgnj_s", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fsgnjn_s", atlas::Action::createAction<&RvfInsts::fsgnjn_sHandler_<XLEN>, RvfInsts>(
                            nullptr, "fsgnjn_s", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fsgnjx_s", atlas::Action::createAction<&RvfInsts::fsgnjx_sHandler_<XLEN>, RvfInsts>(
                            nullptr, "fsgnjx_s", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fsqrt_s", atlas::Action::createAction<&RvfInsts::fsqrt_sHandler_<XLEN>, RvfInsts>(
                           nullptr, "fsqrt_s", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fsub_s", atlas::Action::createAction<&RvfInsts::fsub_sHandler_<XLEN>, RvfInsts>(
                          nullptr, "fsub_s", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fsw", atlas::Action::createAction<&RvfInsts::floatLsHandler<SP, false>, RvfInstsBase>(
                       nullptr, "fsw", ActionTags::EXECUTE_TAG));
    }

    template void RvfInsts::getInstComputeAddressHandlers<RV32>(std::map<std::string, Action> &);
    template void RvfInsts::getInstComputeAddressHandlers<RV64>(std::map<std::string, Action> &);
    template void RvfInsts::getInstHandlers<RV32>(std::map<std::string, Action> &);
    template void RvfInsts::getInstHandlers<RV64>(std::map<std::string, Action> &);

    template <typename XLEN>
    Action::ItrType RvfInsts::fsqrt_sHandler_(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        softfloat_roundingMode = getRM<XLEN>(state);
        const uint32_t rs1_val = checkNanBoxing<RV64, SP>(READ_FP_REG<RV64>(state, inst->getRs1()));
        WRITE_FP_REG<RV64>(state, inst->getRd(),
                           nanBoxing<RV64, SP>(f32_sqrt(float32_t{rs1_val}).v));
        updateCsr<XLEN>(state);
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvfInsts::fsub_sHandler_(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        softfloat_roundingMode = getRM<XLEN>(state);
        const uint32_t rs1_val = checkNanBoxing<RV64, SP>(READ_FP_REG<RV64>(state, inst->getRs1()));
        const uint32_t rs2_val = checkNanBoxing<RV64, SP>(READ_FP_REG<RV64>(state, inst->getRs2()));
        WRITE_FP_REG<RV64>(state, inst->getRd(),
                           nanBoxing<RV64, SP>(f32_sub(float32_t{rs1_val}, float32_t{rs2_val}).v));
        updateCsr<XLEN>(state);
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvfInsts::fnmsub_sHandler_(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        softfloat_roundingMode = getRM<XLEN>(state);
        const uint32_t rs1_val = checkNanBoxing<RV64, SP>(READ_FP_REG<RV64>(state, inst->getRs1()));
        const uint32_t rs2_val = checkNanBoxing<RV64, SP>(READ_FP_REG<RV64>(state, inst->getRs2()));
        const uint32_t rs3_val = checkNanBoxing<RV64, SP>(READ_FP_REG<RV64>(state, inst->getRs3()));
        const uint32_t product = f32_mul(float32_t{rs1_val}, float32_t{rs2_val}).v;
        WRITE_FP_REG<RV64>(
            state, inst->getRd(),
            nanBoxing<RV64, SP>(f32_add(float32_t{product ^ (1 << 31)}, float32_t{rs3_val}).v));
        updateCsr<XLEN>(state);
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvfInsts::feq_sHandler_(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint32_t rs1_val = checkNanBoxing<RV64, SP>(READ_FP_REG<RV64>(state, inst->getRs1()));
        const uint32_t rs2_val = checkNanBoxing<RV64, SP>(READ_FP_REG<RV64>(state, inst->getRs2()));
        WRITE_INT_REG<XLEN>(state, inst->getRd(), f32_eq(float32_t{rs1_val}, float32_t{rs2_val}));
        updateCsr<XLEN>(state);
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvfInsts::fclass_sHandler_(atlas::AtlasState* state, Action::ItrType action_it)
    {
        state->getCurrentInst()->markUnimplemented();
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_either_extension('F', EXT_ZFINX);
        // require_fp;
        // WRITE_RD(f32_classify(FRS1_F));

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvfInsts::fmsub_sHandler_(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        softfloat_roundingMode = getRM<XLEN>(state);
        const uint32_t rs1_val = checkNanBoxing<RV64, SP>(READ_FP_REG<RV64>(state, inst->getRs1()));
        const uint32_t rs2_val = checkNanBoxing<RV64, SP>(READ_FP_REG<RV64>(state, inst->getRs2()));
        const uint32_t rs3_val = checkNanBoxing<RV64, SP>(READ_FP_REG<RV64>(state, inst->getRs3()));
        WRITE_FP_REG<RV64>(
            state, inst->getRd(),
            nanBoxing<RV64, SP>(
                f32_sub(f32_mul(float32_t{rs1_val}, float32_t{rs2_val}), float32_t{rs3_val}).v));
        updateCsr<XLEN>(state);
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvfInsts::fmin_sHandler_(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint32_t rs1_val = checkNanBoxing<RV64, SP>(READ_FP_REG<RV64>(state, inst->getRs1()));
        const uint32_t rs2_val = checkNanBoxing<RV64, SP>(READ_FP_REG<RV64>(state, inst->getRs2()));
        uint32_t rd_val = f32_le_quiet(float32_t{rs1_val}, float32_t{rs2_val}) ? rs1_val : rs2_val;
        fmaxFminNanZeroCheck<SP>(rs1_val, rs2_val, rd_val, false);
        WRITE_FP_REG<RV64>(state, inst->getRd(), nanBoxing<RV64, SP>(rd_val));
        updateCsr<XLEN>(state);
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvfInsts::fmv_w_xHandler_(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint64_t rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        WRITE_FP_REG<RV64>(state, inst->getRd(), nanBoxing<RV64, SP>(rs1_val));
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvfInsts::fcvt_lu_sHandler_(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint32_t rs1_val = checkNanBoxing<RV64, SP>(READ_FP_REG<RV64>(state, inst->getRs1()));
        WRITE_INT_REG<XLEN>(state, inst->getRd(),
                            f32_to_ui64(float32_t{rs1_val}, getRM<XLEN>(state), true));
        updateCsr<XLEN>(state);
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvfInsts::fcvt_s_wHandler_(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint32_t rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        WRITE_FP_REG<RV64>(state, inst->getRd(), nanBoxing<RV64, SP>(i32_to_f32(rs1_val).v));
        updateCsr<XLEN>(state);
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvfInsts::fnmadd_sHandler_(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        softfloat_roundingMode = getRM<XLEN>(state);
        const uint32_t rs1_val = checkNanBoxing<RV64, SP>(READ_FP_REG<RV64>(state, inst->getRs1()));
        const uint32_t rs2_val = checkNanBoxing<RV64, SP>(READ_FP_REG<RV64>(state, inst->getRs2()));
        const uint32_t rs3_val = checkNanBoxing<RV64, SP>(READ_FP_REG<RV64>(state, inst->getRs3()));
        const uint32_t product = f32_mul(float32_t{rs1_val}, float32_t{rs2_val}).v;
        WRITE_FP_REG<RV64>(
            state, inst->getRd(),
            nanBoxing<RV64, SP>(f32_sub(float32_t{product ^ (1 << 31)}, float32_t{rs3_val}).v));
        updateCsr<XLEN>(state);
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvfInsts::fcvt_s_lHandler_(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint64_t rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        WRITE_FP_REG<RV64>(state, inst->getRd(), nanBoxing<RV64, SP>(i64_to_f32(rs1_val).v));
        updateCsr<XLEN>(state);
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvfInsts::fadd_sHandler_(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        softfloat_roundingMode = getRM<XLEN>(state);
        const uint32_t rs1_val = checkNanBoxing<RV64, SP>(READ_FP_REG<RV64>(state, inst->getRs1()));
        const uint32_t rs2_val = checkNanBoxing<RV64, SP>(READ_FP_REG<RV64>(state, inst->getRs2()));
        WRITE_FP_REG<RV64>(state, inst->getRd(),
                           nanBoxing<RV64, SP>(f32_add(float32_t{rs1_val}, float32_t{rs2_val}).v));
        updateCsr<XLEN>(state);
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvfInsts::fmv_x_wHandler_(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint32_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        WRITE_INT_REG<XLEN>(state, inst->getRd(), signExtend<uint32_t, uint64_t>(rs1_val));
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvfInsts::fmax_sHandler_(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint32_t rs1_val = checkNanBoxing<RV64, SP>(READ_FP_REG<RV64>(state, inst->getRs1()));
        const uint32_t rs2_val = checkNanBoxing<RV64, SP>(READ_FP_REG<RV64>(state, inst->getRs2()));
        uint32_t rd_val = f32_le_quiet(float32_t{rs1_val}, float32_t{rs2_val}) ? rs2_val : rs1_val;
        fmaxFminNanZeroCheck<SP>(rs1_val, rs2_val, rd_val, true);
        WRITE_FP_REG<RV64>(state, inst->getRd(), nanBoxing<RV64, SP>(rd_val));
        updateCsr<XLEN>(state);
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvfInsts::fsgnjx_sHandler_(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint32_t rs1_val = checkNanBoxing<RV64, SP>(READ_FP_REG<RV64>(state, inst->getRs1()));
        const uint32_t rs2_val = checkNanBoxing<RV64, SP>(READ_FP_REG<RV64>(state, inst->getRs2()));
        constexpr uint32_t sign_mask = 1 << 31;
        WRITE_FP_REG<RV64>(
            state, inst->getRd(),
            nanBoxing<RV64, SP>((rs1_val & ~sign_mask) | ((rs1_val ^ rs2_val) & sign_mask)));
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvfInsts::fmadd_sHandler_(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        softfloat_roundingMode = getRM<XLEN>(state);
        const uint32_t rs1_val = checkNanBoxing<RV64, SP>(READ_FP_REG<RV64>(state, inst->getRs1()));
        const uint32_t rs2_val = checkNanBoxing<RV64, SP>(READ_FP_REG<RV64>(state, inst->getRs2()));
        const uint32_t rs3_val = checkNanBoxing<RV64, SP>(READ_FP_REG<RV64>(state, inst->getRs3()));
        WRITE_FP_REG<RV64>(
            state, inst->getRd(),
            nanBoxing<RV64, SP>(
                f32_mulAdd(float32_t{rs1_val}, float32_t{rs2_val}, float32_t{rs3_val}).v));
        updateCsr<XLEN>(state);
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvfInsts::fmul_sHandler_(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        softfloat_roundingMode = getRM<XLEN>(state);
        const uint32_t rs1_val = checkNanBoxing<RV64, SP>(READ_FP_REG<RV64>(state, inst->getRs1()));
        const uint32_t rs2_val = checkNanBoxing<RV64, SP>(READ_FP_REG<RV64>(state, inst->getRs2()));
        WRITE_FP_REG<RV64>(state, inst->getRd(),
                           nanBoxing<RV64, SP>(f32_mul(float32_t{rs1_val}, float32_t{rs2_val}).v));
        updateCsr<XLEN>(state);
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvfInsts::flt_sHandler_(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint32_t rs1_val = checkNanBoxing<RV64, SP>(READ_FP_REG<RV64>(state, inst->getRs1()));
        const uint32_t rs2_val = checkNanBoxing<RV64, SP>(READ_FP_REG<RV64>(state, inst->getRs2()));
        WRITE_INT_REG<XLEN>(state, inst->getRd(), f32_lt(float32_t{rs1_val}, float32_t{rs2_val}));
        updateCsr<XLEN>(state);
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvfInsts::fcvt_w_sHandler_(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint32_t rs1_val = checkNanBoxing<RV64, SP>(READ_FP_REG<RV64>(state, inst->getRs1()));
        WRITE_INT_REG<XLEN>(state, inst->getRd(),
                            signExtend<uint32_t, uint64_t>(
                                f32_to_i32(float32_t{rs1_val}, getRM<XLEN>(state), true)));
        updateCsr<XLEN>(state);
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvfInsts::fcvt_l_sHandler_(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint32_t rs1_val = checkNanBoxing<RV64, SP>(READ_FP_REG<RV64>(state, inst->getRs1()));
        WRITE_INT_REG<XLEN>(state, inst->getRd(),
                            f32_to_i64(float32_t{rs1_val}, getRM<XLEN>(state), true));
        updateCsr<XLEN>(state);
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvfInsts::fsgnjn_sHandler_(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint32_t rs1_val = checkNanBoxing<RV64, SP>(READ_FP_REG<RV64>(state, inst->getRs1()));
        const uint32_t rs2_val = checkNanBoxing<RV64, SP>(READ_FP_REG<RV64>(state, inst->getRs2()));
        const uint32_t sign_mask = 1 << 31;
        WRITE_FP_REG<RV64>(
            state, inst->getRd(),
            nanBoxing<RV64, SP>((rs1_val & ~sign_mask) | ((rs2_val & sign_mask) ^ sign_mask)));
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvfInsts::fcvt_s_luHandler_(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint64_t rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        WRITE_FP_REG<RV64>(state, inst->getRd(), nanBoxing<RV64, SP>(ui64_to_f32(rs1_val).v));
        updateCsr<XLEN>(state);
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvfInsts::fcvt_wu_sHandler_(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint32_t rs1_val = checkNanBoxing<RV64, SP>(READ_FP_REG<RV64>(state, inst->getRs1()));
        WRITE_INT_REG<XLEN>(state, inst->getRd(),
                            signExtend<uint32_t, uint64_t>(
                                f32_to_ui32(float32_t{rs1_val}, getRM<XLEN>(state), true)));
        updateCsr<XLEN>(state);
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvfInsts::fdiv_sHandler_(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        softfloat_roundingMode = getRM<XLEN>(state);
        const uint32_t rs1_val = checkNanBoxing<RV64, SP>(READ_FP_REG<RV64>(state, inst->getRs1()));
        const uint32_t rs2_val = checkNanBoxing<RV64, SP>(READ_FP_REG<RV64>(state, inst->getRs2()));
        WRITE_FP_REG<RV64>(state, inst->getRd(),
                           nanBoxing<RV64, SP>(f32_div(float32_t{rs1_val}, float32_t{rs2_val}).v));
        updateCsr<XLEN>(state);
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvfInsts::fsgnj_sHandler_(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint32_t rs1_val = checkNanBoxing<RV64, SP>(READ_FP_REG<RV64>(state, inst->getRs1()));
        const uint32_t rs2_val = checkNanBoxing<RV64, SP>(READ_FP_REG<RV64>(state, inst->getRs2()));
        const uint32_t sign_mask = 1 << 31;
        WRITE_FP_REG<RV64>(state, inst->getRd(),
                           nanBoxing<RV64, SP>((rs1_val & ~sign_mask) | (rs2_val & sign_mask)));
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvfInsts::fcvt_s_wuHandler_(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint32_t rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        WRITE_FP_REG<RV64>(state, inst->getRd(), nanBoxing<RV64, SP>(ui32_to_f32(rs1_val).v));
        updateCsr<XLEN>(state);
        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvfInsts::fle_sHandler_(atlas::AtlasState* state, Action::ItrType action_it)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint32_t rs1_val = checkNanBoxing<RV64, SP>(READ_FP_REG<RV64>(state, inst->getRs1()));
        const uint32_t rs2_val = checkNanBoxing<RV64, SP>(READ_FP_REG<RV64>(state, inst->getRs2()));
        WRITE_INT_REG<XLEN>(state, inst->getRd(), f32_le(float32_t{rs1_val}, float32_t{rs2_val}));
        updateCsr<XLEN>(state);
        return ++action_it;
    }

} // namespace atlas
