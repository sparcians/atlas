#include "core/inst_handlers/f/RvfInsts.hpp"
#include "core/inst_handlers/f/inst_helpers.hpp"
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
        if constexpr (std::is_same_v<XLEN, RV64>)
        {
            inst_handlers.emplace(
                "flw",
                atlas::Action::createAction<&RvfInsts::flw_64_compute_address_handler, RvfInsts>(
                    nullptr, "flw", ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "fsw",
                atlas::Action::createAction<&RvfInsts::fsw_64_compute_address_handler, RvfInsts>(
                    nullptr, "fsw", ActionTags::COMPUTE_ADDR_TAG));
        }
        else if constexpr (std::is_same_v<XLEN, RV32>)
        {
            // RV32 is not supported yet
            static_assert(std::is_same_v<XLEN, RV32> == false);
        }
    }

    template <typename XLEN>
    void RvfInsts::getInstHandlers(std::map<std::string, Action> & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);
        if constexpr (std::is_same_v<XLEN, RV64>)
        {
            inst_handlers.emplace(
                "fadd.s", atlas::Action::createAction<&RvfInsts::fadd_s_64_handler, RvfInsts>(
                              nullptr, "fadd_s", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "fclass.s", atlas::Action::createAction<&RvfInsts::fclass_s_64_handler, RvfInsts>(
                                nullptr, "fclass_s", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "fcvt.l.s", atlas::Action::createAction<&RvfInsts::fcvt_l_s_64_handler, RvfInsts>(
                                nullptr, "fcvt_l_s", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "fcvt.lu.s", atlas::Action::createAction<&RvfInsts::fcvt_lu_s_64_handler, RvfInsts>(
                                 nullptr, "fcvt_lu_s", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "fcvt.s.l", atlas::Action::createAction<&RvfInsts::fcvt_s_l_64_handler, RvfInsts>(
                                nullptr, "fcvt_s_l", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "fcvt.s.lu", atlas::Action::createAction<&RvfInsts::fcvt_s_lu_64_handler, RvfInsts>(
                                 nullptr, "fcvt_s_lu", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "fcvt.s.w", atlas::Action::createAction<&RvfInsts::fcvt_s_w_64_handler, RvfInsts>(
                                nullptr, "fcvt_s_w", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "fcvt.s.wu", atlas::Action::createAction<&RvfInsts::fcvt_s_wu_64_handler, RvfInsts>(
                                 nullptr, "fcvt_s_wu", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "fcvt.w.s", atlas::Action::createAction<&RvfInsts::fcvt_w_s_64_handler, RvfInsts>(
                                nullptr, "fcvt_w_s", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "fcvt.wu.s", atlas::Action::createAction<&RvfInsts::fcvt_wu_s_64_handler, RvfInsts>(
                                 nullptr, "fcvt_wu_s", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "fdiv.s", atlas::Action::createAction<&RvfInsts::fdiv_s_64_handler, RvfInsts>(
                              nullptr, "fdiv_s", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "feq.s", atlas::Action::createAction<&RvfInsts::feq_s_64_handler, RvfInsts>(
                             nullptr, "feq_s", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "fle.s", atlas::Action::createAction<&RvfInsts::fle_s_64_handler, RvfInsts>(
                             nullptr, "fle_s", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "flt.s", atlas::Action::createAction<&RvfInsts::flt_s_64_handler, RvfInsts>(
                             nullptr, "flt_s", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("flw",
                                  atlas::Action::createAction<&RvfInsts::flw_64_handler, RvfInsts>(
                                      nullptr, "flw", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "fmadd.s", atlas::Action::createAction<&RvfInsts::fmadd_s_64_handler, RvfInsts>(
                               nullptr, "fmadd_s", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "fmax.s", atlas::Action::createAction<&RvfInsts::fmax_s_64_handler, RvfInsts>(
                              nullptr, "fmax_s", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "fmin.s", atlas::Action::createAction<&RvfInsts::fmin_s_64_handler, RvfInsts>(
                              nullptr, "fmin_s", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "fmsub.s", atlas::Action::createAction<&RvfInsts::fmsub_s_64_handler, RvfInsts>(
                               nullptr, "fmsub_s", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "fmul.s", atlas::Action::createAction<&RvfInsts::fmul_s_64_handler, RvfInsts>(
                              nullptr, "fmul_s", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "fmv.w.x", atlas::Action::createAction<&RvfInsts::fmv_w_x_64_handler, RvfInsts>(
                               nullptr, "fmv_w_x", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "fmv.x.w", atlas::Action::createAction<&RvfInsts::fmv_x_w_64_handler, RvfInsts>(
                               nullptr, "fmv_x_w", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "fnmadd.s", atlas::Action::createAction<&RvfInsts::fnmadd_s_64_handler, RvfInsts>(
                                nullptr, "fnmadd_s", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "fnmsub.s", atlas::Action::createAction<&RvfInsts::fnmsub_s_64_handler, RvfInsts>(
                                nullptr, "fnmsub_s", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "fsgnj.s", atlas::Action::createAction<&RvfInsts::fsgnj_s_64_handler, RvfInsts>(
                               nullptr, "fsgnj_s", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "fsgnjn.s", atlas::Action::createAction<&RvfInsts::fsgnjn_s_64_handler, RvfInsts>(
                                nullptr, "fsgnjn_s", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "fsgnjx.s", atlas::Action::createAction<&RvfInsts::fsgnjx_s_64_handler, RvfInsts>(
                                nullptr, "fsgnjx_s", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "fsqrt.s", atlas::Action::createAction<&RvfInsts::fsqrt_s_64_handler, RvfInsts>(
                               nullptr, "fsqrt_s", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "fsub.s", atlas::Action::createAction<&RvfInsts::fsub_s_64_handler, RvfInsts>(
                              nullptr, "fsub_s", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("fsw",
                                  atlas::Action::createAction<&RvfInsts::fsw_64_handler, RvfInsts>(
                                      nullptr, "fsw", ActionTags::EXECUTE_TAG));
        }
        else if constexpr (std::is_same_v<XLEN, RV32>)
        {
            sparta_assert(false, "RV32 is not supported yet!");
        }
    }

    // template void RvfInsts::getInstComputeAddressHandlers<RV32>(std::map<std::string, Action> &);
    template void RvfInsts::getInstComputeAddressHandlers<RV64>(std::map<std::string, Action> &);
    // template void RvfInsts::getInstHandlers<RV32>(std::map<std::string, Action> &);
    template void RvfInsts::getInstHandlers<RV64>(std::map<std::string, Action> &);

    ActionGroup* RvfInsts::fsqrt_s_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        softfloat_roundingMode = getRM<RV64>(inst, state);
        const uint32_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        WRITE_FP_REG<RV64>(state, inst->getRd(), f32_sqrt(float32_t{rs1_val}).v);
        update_csr<RV64>(state);
        return nullptr;
    }

    ActionGroup* RvfInsts::fsub_s_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        softfloat_roundingMode = getRM<RV64>(inst, state);
        const uint32_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        const uint32_t rs2_val = READ_FP_REG<RV64>(state, inst->getRs2());
        inst->getRdReg()->dmiWrite(f32_sub(float32_t{rs1_val}, float32_t{rs2_val}).v);
        update_csr<RV64>(state);
        return nullptr;
    }

    ActionGroup* RvfInsts::fnmsub_s_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        softfloat_roundingMode = getRM<RV64>(inst, state);
        const uint32_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        const uint32_t rs2_val = READ_FP_REG<RV64>(state, inst->getRs2());
        const uint32_t rs3_val = READ_FP_REG<RV64>(state, inst->getRs3());
        const uint32_t product = f32_mul(float32_t{rs1_val}, float32_t{rs2_val}).v;
        WRITE_FP_REG<RV64>(state, inst->getRd(),
                           f32_add(float32_t{product ^ (1 << 31)}, float32_t{rs3_val}).v);
        update_csr<RV64>(state);
        return nullptr;
    }

    ActionGroup* RvfInsts::feq_s_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint32_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        const uint32_t rs2_val = READ_FP_REG<RV64>(state, inst->getRs2());
        WRITE_INT_REG<RV64>(state, inst->getRd(), f32_eq(float32_t{rs1_val}, float32_t{rs2_val}));
        update_csr<RV64>(state);
        return nullptr;
    }

    ActionGroup* RvfInsts::fclass_s_64_handler(atlas::AtlasState* state)
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

    ActionGroup* RvfInsts::fmsub_s_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        softfloat_roundingMode = getRM<RV64>(inst, state);
        const uint32_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        const uint32_t rs2_val = READ_FP_REG<RV64>(state, inst->getRs2());
        const uint32_t rs3_val = READ_FP_REG<RV64>(state, inst->getRs3());
        WRITE_FP_REG<RV64>(
            state, inst->getRd(),
            f32_sub(f32_mul(float32_t{rs1_val}, float32_t{rs2_val}), float32_t{rs3_val}).v);
        update_csr<RV64>(state);
        return nullptr;
    }

    ActionGroup* RvfInsts::fmin_s_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint32_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        const uint32_t rs2_val = READ_FP_REG<RV64>(state, inst->getRs2());
        uint32_t rd_val = f32_le_quiet(float32_t{rs1_val}, float32_t{rs2_val}) ? rs1_val : rs2_val;
        fmax_fmin_nan_zero_check<SP>(rs1_val, rs2_val, rd_val, false);
        WRITE_FP_REG<RV64>(state, inst->getRd(), rd_val);
        update_csr<RV64>(state);
        return nullptr;
    }

    ActionGroup* RvfInsts::fmv_w_x_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint64_t rs1_val = READ_INT_REG<RV64>(state, inst->getRs1());
        constexpr uint64_t rv64_low32_mask = 0xFFFFFFFF;
        WRITE_FP_REG<RV64>(state, inst->getRd(), rs1_val & rv64_low32_mask);
        return nullptr;
    }

    ActionGroup* RvfInsts::fcvt_lu_s_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint32_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        WRITE_INT_REG<RV64>(state, inst->getRd(),
                            f32_to_ui64(float32_t{rs1_val}, getRM<RV64>(inst, state), true));
        update_csr<RV64>(state);
        return nullptr;
    }

    ActionGroup* RvfInsts::fcvt_s_w_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint32_t rs1_val = READ_INT_REG<RV64>(state, inst->getRs1());
        WRITE_FP_REG<RV64>(state, inst->getRd(), i32_to_f32(rs1_val).v);
        update_csr<RV64>(state);
        return nullptr;
    }

    ActionGroup* RvfInsts::fnmadd_s_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        softfloat_roundingMode = getRM<RV64>(inst, state);
        const uint32_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        const uint32_t rs2_val = READ_FP_REG<RV64>(state, inst->getRs2());
        const uint32_t rs3_val = READ_FP_REG<RV64>(state, inst->getRs3());
        const uint32_t product = f32_mul(float32_t{rs1_val}, float32_t{rs2_val}).v;
        WRITE_FP_REG<RV64>(state, inst->getRd(),
                           f32_sub(float32_t{product ^ (1 << 31)}, float32_t{rs3_val}).v);
        update_csr<RV64>(state);
        return nullptr;
    }

    ActionGroup* RvfInsts::fcvt_s_l_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint64_t rs1_val = READ_INT_REG<RV64>(state, inst->getRs1());
        WRITE_FP_REG<RV64>(state, inst->getRd(), i64_to_f32(rs1_val).v);
        update_csr<RV64>(state);
        return nullptr;
    }

    ActionGroup* RvfInsts::fadd_s_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        softfloat_roundingMode = getRM<RV64>(inst, state);
        const uint32_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        const uint32_t rs2_val = READ_FP_REG<RV64>(state, inst->getRs2());
        WRITE_FP_REG<RV64>(state, inst->getRd(), f32_add(float32_t{rs1_val}, float32_t{rs2_val}).v);
        update_csr<RV64>(state);
        return nullptr;
    }

    ActionGroup* RvfInsts::fsw_64_compute_address_handler(atlas::AtlasState* state)
    {
        return compute_address_handler<RV64>(state);
    }

    ActionGroup* RvfInsts::fsw_64_handler(atlas::AtlasState* state)
    {
        return float_ls_handler<RV64, SP, false>(state);
    }

    ActionGroup* RvfInsts::fmv_x_w_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint32_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        WRITE_INT_REG<RV64>(state, inst->getRd(), signExtend<uint32_t, uint64_t>(rs1_val));
        return nullptr;
    }

    ActionGroup* RvfInsts::fmax_s_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint32_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        const uint32_t rs2_val = READ_FP_REG<RV64>(state, inst->getRs2());
        uint32_t rd_val = f32_le_quiet(float32_t{rs1_val}, float32_t{rs2_val}) ? rs2_val : rs1_val;
        fmax_fmin_nan_zero_check<SP>(rs1_val, rs2_val, rd_val, true);
        WRITE_FP_REG<RV64>(state, inst->getRd(), rd_val);
        update_csr<RV64>(state);
        return nullptr;
    }

    ActionGroup* RvfInsts::fsgnjx_s_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint32_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        const uint32_t rs2_val = READ_FP_REG<RV64>(state, inst->getRs2());
        constexpr uint32_t sign_mask = 1 << 31;
        WRITE_FP_REG<RV64>(state, inst->getRd(),
                           (rs1_val & ~sign_mask) | ((rs1_val ^ rs2_val) & sign_mask));
        return nullptr;
    }

    ActionGroup* RvfInsts::flw_64_compute_address_handler(atlas::AtlasState* state)
    {
        return compute_address_handler<RV64>(state);
    }

    ActionGroup* RvfInsts::flw_64_handler(atlas::AtlasState* state)
    {
        return float_ls_handler<RV64, SP, true>(state);
    }

    ActionGroup* RvfInsts::fmadd_s_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        softfloat_roundingMode = getRM<RV64>(inst, state);
        const uint32_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        const uint32_t rs2_val = READ_FP_REG<RV64>(state, inst->getRs2());
        const uint32_t rs3_val = READ_FP_REG<RV64>(state, inst->getRs3());
        WRITE_FP_REG<RV64>(
            state, inst->getRd(),
            f32_mulAdd(float32_t{rs1_val}, float32_t{rs2_val}, float32_t{rs3_val}).v);
        update_csr<RV64>(state);
        return nullptr;
    }

    ActionGroup* RvfInsts::fmul_s_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        softfloat_roundingMode = getRM<RV64>(inst, state);
        const uint32_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        const uint32_t rs2_val = READ_FP_REG<RV64>(state, inst->getRs2());
        WRITE_FP_REG<RV64>(state, inst->getRd(), f32_mul(float32_t{rs1_val}, float32_t{rs2_val}).v);
        update_csr<RV64>(state);
        return nullptr;
    }

    ActionGroup* RvfInsts::flt_s_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint32_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        const uint32_t rs2_val = READ_FP_REG<RV64>(state, inst->getRs2());
        WRITE_INT_REG<RV64>(state, inst->getRd(), f32_lt(float32_t{rs1_val}, float32_t{rs2_val}));
        update_csr<RV64>(state);
        return nullptr;
    }

    ActionGroup* RvfInsts::fcvt_w_s_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint32_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        WRITE_INT_REG<RV64>(state, inst->getRd(),
                            signExtend<uint32_t, uint64_t>(
                                f32_to_i32(float32_t{rs1_val}, getRM<RV64>(inst, state), true)));
        update_csr<RV64>(state);
        return nullptr;
    }

    ActionGroup* RvfInsts::fcvt_l_s_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint32_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        WRITE_INT_REG<RV64>(state, inst->getRd(),
                            f32_to_i64(float32_t{rs1_val}, getRM<RV64>(inst, state), true));
        update_csr<RV64>(state);
        return nullptr;
    }

    ActionGroup* RvfInsts::fsgnjn_s_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint32_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        const uint32_t rs2_val = READ_FP_REG<RV64>(state, inst->getRs2());
        const uint32_t sign_mask = 1 << 31;
        WRITE_FP_REG<RV64>(state, inst->getRd(),
                           (rs1_val & ~sign_mask) | ((rs2_val & sign_mask) ^ sign_mask));
        return nullptr;
    }

    ActionGroup* RvfInsts::fcvt_s_lu_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint64_t rs1_val = READ_INT_REG<RV64>(state, inst->getRs1());
        WRITE_FP_REG<RV64>(state, inst->getRd(), ui64_to_f32(rs1_val).v);
        update_csr<RV64>(state);
        return nullptr;
    }

    ActionGroup* RvfInsts::fcvt_wu_s_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint32_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        WRITE_INT_REG<RV64>(state, inst->getRd(),
                            signExtend<uint32_t, uint64_t>(
                                f32_to_ui32(float32_t{rs1_val}, getRM<RV64>(inst, state), true)));
        update_csr<RV64>(state);
        return nullptr;
    }

    ActionGroup* RvfInsts::fdiv_s_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        softfloat_roundingMode = getRM<RV64>(inst, state);
        const uint32_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        const uint32_t rs2_val = READ_FP_REG<RV64>(state, inst->getRs2());
        WRITE_FP_REG<RV64>(state, inst->getRd(), f32_div(float32_t{rs1_val}, float32_t{rs2_val}).v);
        update_csr<RV64>(state);
        return nullptr;
    }

    ActionGroup* RvfInsts::fsgnj_s_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint32_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        const uint32_t rs2_val = READ_FP_REG<RV64>(state, inst->getRs2());
        const uint32_t sign_mask = 1 << 31;
        WRITE_FP_REG<RV64>(state, inst->getRd(), (rs1_val & ~sign_mask) | (rs2_val & sign_mask));
        return nullptr;
    }

    ActionGroup* RvfInsts::fcvt_s_wu_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint32_t rs1_val = READ_INT_REG<RV64>(state, inst->getRs1());
        WRITE_FP_REG<RV64>(state, inst->getRd(), ui32_to_f32(rs1_val).v);
        update_csr<RV64>(state);
        return nullptr;
    }

    ActionGroup* RvfInsts::fle_s_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint32_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        const uint32_t rs2_val = READ_FP_REG<RV64>(state, inst->getRs2());
        WRITE_INT_REG<RV64>(state, inst->getRd(), f32_le(float32_t{rs1_val}, float32_t{rs2_val}));
        update_csr<RV64>(state);
        return nullptr;
    }

} // namespace atlas
