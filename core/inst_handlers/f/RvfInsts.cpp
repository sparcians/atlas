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
            "flw", atlas::Action::createAction<&RvfInsts::computeAddressHandler<XLEN>, RvfInsts>(
                       nullptr, "flw", ActionTags::COMPUTE_ADDR_TAG));
        inst_handlers.emplace(
            "fsw", atlas::Action::createAction<&RvfInsts::computeAddressHandler<XLEN>, RvfInsts>(
                       nullptr, "fsw", ActionTags::COMPUTE_ADDR_TAG));
    }

    template <typename XLEN>
    void RvfInsts::getInstHandlers(std::map<std::string, Action> & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);
        inst_handlers.emplace(
            "fadd.s", atlas::Action::createAction<&RvfInsts::fadd_s_handler<XLEN>, RvfInsts>(
                          nullptr, "fadd_s", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fclass.s", atlas::Action::createAction<&RvfInsts::fclass_s_handler<XLEN>, RvfInsts>(
                            nullptr, "fclass_s", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fcvt.l.s", atlas::Action::createAction<&RvfInsts::fcvt_l_s_handler<XLEN>, RvfInsts>(
                            nullptr, "fcvt_l_s", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fcvt.lu.s", atlas::Action::createAction<&RvfInsts::fcvt_lu_s_handler<XLEN>, RvfInsts>(
                             nullptr, "fcvt_lu_s", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fcvt.s.l", atlas::Action::createAction<&RvfInsts::fcvt_s_l_handler<XLEN>, RvfInsts>(
                            nullptr, "fcvt_s_l", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fcvt.s.lu", atlas::Action::createAction<&RvfInsts::fcvt_s_lu_handler<XLEN>, RvfInsts>(
                             nullptr, "fcvt_s_lu", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fcvt.s.w", atlas::Action::createAction<&RvfInsts::fcvt_s_w_handler<XLEN>, RvfInsts>(
                            nullptr, "fcvt_s_w", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fcvt.s.wu", atlas::Action::createAction<&RvfInsts::fcvt_s_wu_handler<XLEN>, RvfInsts>(
                             nullptr, "fcvt_s_wu", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fcvt.w.s", atlas::Action::createAction<&RvfInsts::fcvt_w_s_handler<XLEN>, RvfInsts>(
                            nullptr, "fcvt_w_s", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fcvt.wu.s", atlas::Action::createAction<&RvfInsts::fcvt_wu_s_handler<XLEN>, RvfInsts>(
                             nullptr, "fcvt_wu_s", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fdiv.s", atlas::Action::createAction<&RvfInsts::fdiv_s_handler<XLEN>, RvfInsts>(
                          nullptr, "fdiv_s", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("feq.s",
                              atlas::Action::createAction<&RvfInsts::feq_s_handler<XLEN>, RvfInsts>(
                                  nullptr, "feq_s", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("fle.s",
                              atlas::Action::createAction<&RvfInsts::fle_s_handler<XLEN>, RvfInsts>(
                                  nullptr, "fle_s", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace("flt.s",
                              atlas::Action::createAction<&RvfInsts::flt_s_handler<XLEN>, RvfInsts>(
                                  nullptr, "flt_s", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "flw", atlas::Action::createAction<&RvfInsts::floatLsHandler<SP, true>, RvfInsts>(
                       nullptr, "flw", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fmadd.s", atlas::Action::createAction<&RvfInsts::fmadd_s_handler<XLEN>, RvfInsts>(
                           nullptr, "fmadd_s", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fmax.s", atlas::Action::createAction<&RvfInsts::fmax_s_handler<XLEN>, RvfInsts>(
                          nullptr, "fmax_s", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fmin.s", atlas::Action::createAction<&RvfInsts::fmin_s_handler<XLEN>, RvfInsts>(
                          nullptr, "fmin_s", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fmsub.s", atlas::Action::createAction<&RvfInsts::fmsub_s_handler<XLEN>, RvfInsts>(
                           nullptr, "fmsub_s", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fmul.s", atlas::Action::createAction<&RvfInsts::fmul_s_handler<XLEN>, RvfInsts>(
                          nullptr, "fmul_s", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fmv.w.x", atlas::Action::createAction<&RvfInsts::fmv_w_x_handler<XLEN>, RvfInsts>(
                           nullptr, "fmv_w_x", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fmv.x.w", atlas::Action::createAction<&RvfInsts::fmv_x_w_handler<XLEN>, RvfInsts>(
                           nullptr, "fmv_x_w", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fnmadd.s", atlas::Action::createAction<&RvfInsts::fnmadd_s_handler<XLEN>, RvfInsts>(
                            nullptr, "fnmadd_s", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fnmsub.s", atlas::Action::createAction<&RvfInsts::fnmsub_s_handler<XLEN>, RvfInsts>(
                            nullptr, "fnmsub_s", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fsgnj.s", atlas::Action::createAction<&RvfInsts::fsgnj_s_handler<XLEN>, RvfInsts>(
                           nullptr, "fsgnj_s", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fsgnjn.s", atlas::Action::createAction<&RvfInsts::fsgnjn_s_handler<XLEN>, RvfInsts>(
                            nullptr, "fsgnjn_s", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fsgnjx.s", atlas::Action::createAction<&RvfInsts::fsgnjx_s_handler<XLEN>, RvfInsts>(
                            nullptr, "fsgnjx_s", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fsqrt.s", atlas::Action::createAction<&RvfInsts::fsqrt_s_handler<XLEN>, RvfInsts>(
                           nullptr, "fsqrt_s", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fsub.s", atlas::Action::createAction<&RvfInsts::fsub_s_handler<XLEN>, RvfInsts>(
                          nullptr, "fsub_s", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "fsw", atlas::Action::createAction<&RvfInsts::floatLsHandler<SP, false>, RvfInsts>(
                       nullptr, "fsw", ActionTags::EXECUTE_TAG));
    }

    template void RvfInsts::getInstComputeAddressHandlers<RV32>(std::map<std::string, Action> &);
    template void RvfInsts::getInstComputeAddressHandlers<RV64>(std::map<std::string, Action> &);
    template void RvfInsts::getInstHandlers<RV32>(std::map<std::string, Action> &);
    template void RvfInsts::getInstHandlers<RV64>(std::map<std::string, Action> &);

    template <typename XLEN> ActionGroup* RvfInsts::computeAddressHandler(atlas::AtlasState* state)
    {
        return RvfInstsBase::computeAddressHandler<XLEN>(state);
    }

    template <typename SIZE, bool LOAD>
    ActionGroup* RvfInsts::floatLsHandler(atlas::AtlasState* state)
    {
        return RvfInstsBase::floatLsHandler<SIZE, LOAD>(state);
    }

    template <typename XLEN> ActionGroup* RvfInsts::fsqrt_s_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        softfloat_roundingMode = getRM<XLEN>(state);
        const uint32_t rs1_val = checkNanBoxing<RV64, SP>(READ_FP_REG<RV64>(state, inst->getRs1()));
        WRITE_FP_REG<RV64>(state, inst->getRd(),
                           nanBoxing<RV64, SP>(f32_sqrt(float32_t{rs1_val}).v));
        updateCsr<XLEN>(state);
        return nullptr;
    }

    template <typename XLEN> ActionGroup* RvfInsts::fsub_s_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        softfloat_roundingMode = getRM<XLEN>(state);
        const uint32_t rs1_val = checkNanBoxing<RV64, SP>(READ_FP_REG<RV64>(state, inst->getRs1()));
        const uint32_t rs2_val = checkNanBoxing<RV64, SP>(READ_FP_REG<RV64>(state, inst->getRs2()));
        WRITE_FP_REG<RV64>(state, inst->getRd(),
                           nanBoxing<RV64, SP>(f32_sub(float32_t{rs1_val}, float32_t{rs2_val}).v));
        updateCsr<XLEN>(state);
        return nullptr;
    }

    template <typename XLEN> ActionGroup* RvfInsts::fnmsub_s_handler(atlas::AtlasState* state)
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
        return nullptr;
    }

    template <typename XLEN> ActionGroup* RvfInsts::feq_s_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint32_t rs1_val = checkNanBoxing<RV64, SP>(READ_FP_REG<RV64>(state, inst->getRs1()));
        const uint32_t rs2_val = checkNanBoxing<RV64, SP>(READ_FP_REG<RV64>(state, inst->getRs2()));
        WRITE_INT_REG<XLEN>(state, inst->getRd(), f32_eq(float32_t{rs1_val}, float32_t{rs2_val}));
        updateCsr<XLEN>(state);
        return nullptr;
    }

    template <typename XLEN> ActionGroup* RvfInsts::fclass_s_handler(atlas::AtlasState* state)
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
        return nullptr;
    }

    template <typename XLEN> ActionGroup* RvfInsts::fmsub_s_handler(atlas::AtlasState* state)
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
        return nullptr;
    }

    template <typename XLEN> ActionGroup* RvfInsts::fmin_s_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint32_t rs1_val = checkNanBoxing<RV64, SP>(READ_FP_REG<RV64>(state, inst->getRs1()));
        const uint32_t rs2_val = checkNanBoxing<RV64, SP>(READ_FP_REG<RV64>(state, inst->getRs2()));
        uint32_t rd_val = f32_le_quiet(float32_t{rs1_val}, float32_t{rs2_val}) ? rs1_val : rs2_val;
        fmaxFminNanZeroCheck<SP>(rs1_val, rs2_val, rd_val, false);
        WRITE_FP_REG<RV64>(state, inst->getRd(), nanBoxing<RV64, SP>(rd_val));
        updateCsr<XLEN>(state);
        return nullptr;
    }

    template <typename XLEN> ActionGroup* RvfInsts::fmv_w_x_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint64_t rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        WRITE_FP_REG<RV64>(state, inst->getRd(), nanBoxing<RV64, SP>(rs1_val));
        return nullptr;
    }

    template <typename XLEN> ActionGroup* RvfInsts::fcvt_lu_s_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint32_t rs1_val = checkNanBoxing<RV64, SP>(READ_FP_REG<RV64>(state, inst->getRs1()));
        WRITE_INT_REG<XLEN>(state, inst->getRd(),
                            f32_to_ui64(float32_t{rs1_val}, getRM<XLEN>(state), true));
        updateCsr<XLEN>(state);
        return nullptr;
    }

    template <typename XLEN> ActionGroup* RvfInsts::fcvt_s_w_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint32_t rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        WRITE_FP_REG<RV64>(state, inst->getRd(), nanBoxing<RV64, SP>(i32_to_f32(rs1_val).v));
        updateCsr<XLEN>(state);
        return nullptr;
    }

    template <typename XLEN> ActionGroup* RvfInsts::fnmadd_s_handler(atlas::AtlasState* state)
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
        return nullptr;
    }

    template <typename XLEN> ActionGroup* RvfInsts::fcvt_s_l_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint64_t rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        WRITE_FP_REG<RV64>(state, inst->getRd(), nanBoxing<RV64, SP>(i64_to_f32(rs1_val).v));
        updateCsr<XLEN>(state);
        return nullptr;
    }

    template <typename XLEN> ActionGroup* RvfInsts::fadd_s_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        softfloat_roundingMode = getRM<XLEN>(state);
        const uint32_t rs1_val = checkNanBoxing<RV64, SP>(READ_FP_REG<RV64>(state, inst->getRs1()));
        const uint32_t rs2_val = checkNanBoxing<RV64, SP>(READ_FP_REG<RV64>(state, inst->getRs2()));
        WRITE_FP_REG<RV64>(state, inst->getRd(),
                           nanBoxing<RV64, SP>(f32_add(float32_t{rs1_val}, float32_t{rs2_val}).v));
        updateCsr<XLEN>(state);
        return nullptr;
    }

    template <typename XLEN> ActionGroup* RvfInsts::fmv_x_w_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint32_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        WRITE_INT_REG<XLEN>(state, inst->getRd(), signExtend<uint32_t, uint64_t>(rs1_val));
        return nullptr;
    }

    template <typename XLEN> ActionGroup* RvfInsts::fmax_s_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint32_t rs1_val = checkNanBoxing<RV64, SP>(READ_FP_REG<RV64>(state, inst->getRs1()));
        const uint32_t rs2_val = checkNanBoxing<RV64, SP>(READ_FP_REG<RV64>(state, inst->getRs2()));
        uint32_t rd_val = f32_le_quiet(float32_t{rs1_val}, float32_t{rs2_val}) ? rs2_val : rs1_val;
        fmaxFminNanZeroCheck<SP>(rs1_val, rs2_val, rd_val, true);
        WRITE_FP_REG<RV64>(state, inst->getRd(), nanBoxing<RV64, SP>(rd_val));
        updateCsr<XLEN>(state);
        return nullptr;
    }

    template <typename XLEN> ActionGroup* RvfInsts::fsgnjx_s_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint32_t rs1_val = checkNanBoxing<RV64, SP>(READ_FP_REG<RV64>(state, inst->getRs1()));
        const uint32_t rs2_val = checkNanBoxing<RV64, SP>(READ_FP_REG<RV64>(state, inst->getRs2()));
        constexpr uint32_t sign_mask = 1 << 31;
        WRITE_FP_REG<RV64>(
            state, inst->getRd(),
            nanBoxing<RV64, SP>((rs1_val & ~sign_mask) | ((rs1_val ^ rs2_val) & sign_mask)));
        return nullptr;
    }

    template <typename XLEN> ActionGroup* RvfInsts::fmadd_s_handler(atlas::AtlasState* state)
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
        return nullptr;
    }

    template <typename XLEN> ActionGroup* RvfInsts::fmul_s_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        softfloat_roundingMode = getRM<XLEN>(state);
        const uint32_t rs1_val = checkNanBoxing<RV64, SP>(READ_FP_REG<RV64>(state, inst->getRs1()));
        const uint32_t rs2_val = checkNanBoxing<RV64, SP>(READ_FP_REG<RV64>(state, inst->getRs2()));
        WRITE_FP_REG<RV64>(state, inst->getRd(),
                           nanBoxing<RV64, SP>(f32_mul(float32_t{rs1_val}, float32_t{rs2_val}).v));
        updateCsr<XLEN>(state);
        return nullptr;
    }

    template <typename XLEN> ActionGroup* RvfInsts::flt_s_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint32_t rs1_val = checkNanBoxing<RV64, SP>(READ_FP_REG<RV64>(state, inst->getRs1()));
        const uint32_t rs2_val = checkNanBoxing<RV64, SP>(READ_FP_REG<RV64>(state, inst->getRs2()));
        WRITE_INT_REG<XLEN>(state, inst->getRd(), f32_lt(float32_t{rs1_val}, float32_t{rs2_val}));
        updateCsr<XLEN>(state);
        return nullptr;
    }

    template <typename XLEN> ActionGroup* RvfInsts::fcvt_w_s_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint32_t rs1_val = checkNanBoxing<RV64, SP>(READ_FP_REG<RV64>(state, inst->getRs1()));
        WRITE_INT_REG<XLEN>(state, inst->getRd(),
                            signExtend<uint32_t, uint64_t>(
                                f32_to_i32(float32_t{rs1_val}, getRM<XLEN>(state), true)));
        updateCsr<XLEN>(state);
        return nullptr;
    }

    template <typename XLEN> ActionGroup* RvfInsts::fcvt_l_s_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint32_t rs1_val = checkNanBoxing<RV64, SP>(READ_FP_REG<RV64>(state, inst->getRs1()));
        WRITE_INT_REG<XLEN>(state, inst->getRd(),
                            f32_to_i64(float32_t{rs1_val}, getRM<XLEN>(state), true));
        updateCsr<XLEN>(state);
        return nullptr;
    }

    template <typename XLEN> ActionGroup* RvfInsts::fsgnjn_s_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint32_t rs1_val = checkNanBoxing<RV64, SP>(READ_FP_REG<RV64>(state, inst->getRs1()));
        const uint32_t rs2_val = checkNanBoxing<RV64, SP>(READ_FP_REG<RV64>(state, inst->getRs2()));
        const uint32_t sign_mask = 1 << 31;
        WRITE_FP_REG<RV64>(
            state, inst->getRd(),
            nanBoxing<RV64, SP>((rs1_val & ~sign_mask) | ((rs2_val & sign_mask) ^ sign_mask)));
        return nullptr;
    }

    template <typename XLEN> ActionGroup* RvfInsts::fcvt_s_lu_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint64_t rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        WRITE_FP_REG<RV64>(state, inst->getRd(), nanBoxing<RV64, SP>(ui64_to_f32(rs1_val).v));
        updateCsr<XLEN>(state);
        return nullptr;
    }

    template <typename XLEN> ActionGroup* RvfInsts::fcvt_wu_s_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint32_t rs1_val = checkNanBoxing<RV64, SP>(READ_FP_REG<RV64>(state, inst->getRs1()));
        WRITE_INT_REG<XLEN>(state, inst->getRd(),
                            signExtend<uint32_t, uint64_t>(
                                f32_to_ui32(float32_t{rs1_val}, getRM<XLEN>(state), true)));
        updateCsr<XLEN>(state);
        return nullptr;
    }

    template <typename XLEN> ActionGroup* RvfInsts::fdiv_s_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        softfloat_roundingMode = getRM<XLEN>(state);
        const uint32_t rs1_val = checkNanBoxing<RV64, SP>(READ_FP_REG<RV64>(state, inst->getRs1()));
        const uint32_t rs2_val = checkNanBoxing<RV64, SP>(READ_FP_REG<RV64>(state, inst->getRs2()));
        WRITE_FP_REG<RV64>(state, inst->getRd(),
                           nanBoxing<RV64, SP>(f32_div(float32_t{rs1_val}, float32_t{rs2_val}).v));
        updateCsr<XLEN>(state);
        return nullptr;
    }

    template <typename XLEN> ActionGroup* RvfInsts::fsgnj_s_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint32_t rs1_val = checkNanBoxing<RV64, SP>(READ_FP_REG<RV64>(state, inst->getRs1()));
        const uint32_t rs2_val = checkNanBoxing<RV64, SP>(READ_FP_REG<RV64>(state, inst->getRs2()));
        const uint32_t sign_mask = 1 << 31;
        WRITE_FP_REG<RV64>(state, inst->getRd(),
                           nanBoxing<RV64, SP>((rs1_val & ~sign_mask) | (rs2_val & sign_mask)));
        return nullptr;
    }

    template <typename XLEN> ActionGroup* RvfInsts::fcvt_s_wu_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint32_t rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
        WRITE_FP_REG<RV64>(state, inst->getRd(), nanBoxing<RV64, SP>(ui32_to_f32(rs1_val).v));
        updateCsr<XLEN>(state);
        return nullptr;
    }

    template <typename XLEN> ActionGroup* RvfInsts::fle_s_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint32_t rs1_val = checkNanBoxing<RV64, SP>(READ_FP_REG<RV64>(state, inst->getRs1()));
        const uint32_t rs2_val = checkNanBoxing<RV64, SP>(READ_FP_REG<RV64>(state, inst->getRs2()));
        WRITE_INT_REG<XLEN>(state, inst->getRd(), f32_le(float32_t{rs1_val}, float32_t{rs2_val}));
        updateCsr<XLEN>(state);
        return nullptr;
    }

} // namespace atlas
