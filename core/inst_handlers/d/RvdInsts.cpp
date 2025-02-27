#include "core/inst_handlers/d/RvdInsts.hpp"
#include "core/inst_handlers/f/inst_helpers.hpp"
#include "include/ActionTags.hpp"
#include "core/ActionGroup.hpp"

namespace atlas
{
    template <typename XLEN>
    void RvdInsts::getInstComputeAddressHandlers(std::map<std::string, Action> & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);
        if constexpr (std::is_same_v<XLEN, RV64>)
        {
            inst_handlers.emplace(
                "fld",
                atlas::Action::createAction<&RvdInsts::fld_64_compute_address_handler, RvdInsts>(
                    nullptr, "fld", ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "fsd",
                atlas::Action::createAction<&RvdInsts::fsd_64_compute_address_handler, RvdInsts>(
                    nullptr, "fsd", ActionTags::COMPUTE_ADDR_TAG));
        }
        else if constexpr (std::is_same_v<XLEN, RV32>)
        {
            // RV32 is not supported yet
            static_assert(std::is_same_v<XLEN, RV32> == false);
        }
    }

    template <typename XLEN>
    void RvdInsts::getInstHandlers(std::map<std::string, Action> & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);
        if constexpr (std::is_same_v<XLEN, RV64>)
        {
            inst_handlers.emplace(
                "fadd.d", atlas::Action::createAction<&RvdInsts::fadd_d_64_handler, RvdInsts>(
                              nullptr, "fadd_d", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "fclass.d", atlas::Action::createAction<&RvdInsts::fclass_d_64_handler, RvdInsts>(
                                nullptr, "fclass_d", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "fcvt.d.l", atlas::Action::createAction<&RvdInsts::fcvt_d_l_64_handler, RvdInsts>(
                                nullptr, "fcvt_d_l", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "fcvt.d.lu", atlas::Action::createAction<&RvdInsts::fcvt_d_lu_64_handler, RvdInsts>(
                                 nullptr, "fcvt_d_lu", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "fcvt.d.s", atlas::Action::createAction<&RvdInsts::fcvt_d_s_64_handler, RvdInsts>(
                                nullptr, "fcvt_d_s", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "fcvt.d.w", atlas::Action::createAction<&RvdInsts::fcvt_d_w_64_handler, RvdInsts>(
                                nullptr, "fcvt_d_w", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "fcvt.d.wu", atlas::Action::createAction<&RvdInsts::fcvt_d_wu_64_handler, RvdInsts>(
                                 nullptr, "fcvt_d_wu", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "fcvt.l.d", atlas::Action::createAction<&RvdInsts::fcvt_l_d_64_handler, RvdInsts>(
                                nullptr, "fcvt_l_d", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "fcvt.lu.d", atlas::Action::createAction<&RvdInsts::fcvt_lu_d_64_handler, RvdInsts>(
                                 nullptr, "fcvt_lu_d", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "fcvt.s.d", atlas::Action::createAction<&RvdInsts::fcvt_s_d_64_handler, RvdInsts>(
                                nullptr, "fcvt_s_d", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "fcvt.w.d", atlas::Action::createAction<&RvdInsts::fcvt_w_d_64_handler, RvdInsts>(
                                nullptr, "fcvt_w_d", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "fcvt.wu.d", atlas::Action::createAction<&RvdInsts::fcvt_wu_d_64_handler, RvdInsts>(
                                 nullptr, "fcvt_wu_d", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "fdiv.d", atlas::Action::createAction<&RvdInsts::fdiv_d_64_handler, RvdInsts>(
                              nullptr, "fdiv_d", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "feq.d", atlas::Action::createAction<&RvdInsts::feq_d_64_handler, RvdInsts>(
                             nullptr, "feq_d", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("fld",
                                  atlas::Action::createAction<&RvdInsts::fld_64_handler, RvdInsts>(
                                      nullptr, "fld", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "fle.d", atlas::Action::createAction<&RvdInsts::fle_d_64_handler, RvdInsts>(
                             nullptr, "fle_d", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "flt.d", atlas::Action::createAction<&RvdInsts::flt_d_64_handler, RvdInsts>(
                             nullptr, "flt_d", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "fmadd.d", atlas::Action::createAction<&RvdInsts::fmadd_d_64_handler, RvdInsts>(
                               nullptr, "fmadd_d", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "fmax.d", atlas::Action::createAction<&RvdInsts::fmax_d_64_handler, RvdInsts>(
                              nullptr, "fmax_d", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "fmin.d", atlas::Action::createAction<&RvdInsts::fmin_d_64_handler, RvdInsts>(
                              nullptr, "fmin_d", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "fmsub.d", atlas::Action::createAction<&RvdInsts::fmsub_d_64_handler, RvdInsts>(
                               nullptr, "fmsub_d", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "fmul.d", atlas::Action::createAction<&RvdInsts::fmul_d_64_handler, RvdInsts>(
                              nullptr, "fmul_d", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "fmv.d.x", atlas::Action::createAction<&RvdInsts::fmv_d_x_64_handler, RvdInsts>(
                               nullptr, "fmv_d_x", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "fmv.x.d", atlas::Action::createAction<&RvdInsts::fmv_x_d_64_handler, RvdInsts>(
                               nullptr, "fmv_x_d", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "fnmadd.d", atlas::Action::createAction<&RvdInsts::fnmadd_d_64_handler, RvdInsts>(
                                nullptr, "fnmadd_d", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "fnmsub.d", atlas::Action::createAction<&RvdInsts::fnmsub_d_64_handler, RvdInsts>(
                                nullptr, "fnmsub_d", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace("fsd",
                                  atlas::Action::createAction<&RvdInsts::fsd_64_handler, RvdInsts>(
                                      nullptr, "fsd", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "fsgnj.d", atlas::Action::createAction<&RvdInsts::fsgnj_d_64_handler, RvdInsts>(
                               nullptr, "fsgnj_d", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "fsgnjn.d", atlas::Action::createAction<&RvdInsts::fsgnjn_d_64_handler, RvdInsts>(
                                nullptr, "fsgnjn_d", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "fsgnjx.d", atlas::Action::createAction<&RvdInsts::fsgnjx_d_64_handler, RvdInsts>(
                                nullptr, "fsgnjx_d", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "fsqrt.d", atlas::Action::createAction<&RvdInsts::fsqrt_d_64_handler, RvdInsts>(
                               nullptr, "fsqrt_d", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "fsub.d", atlas::Action::createAction<&RvdInsts::fsub_d_64_handler, RvdInsts>(
                              nullptr, "fsub_d", ActionTags::EXECUTE_TAG));
        }
        else if constexpr (std::is_same_v<XLEN, RV32>)
        {
            sparta_assert(false, "RV32 is not supported yet!");
        }
    }

    // template void RvdInsts::getInstComputeAddressHandlers<RV32>(std::map<std::string, Action> &);
    template void RvdInsts::getInstComputeAddressHandlers<RV64>(std::map<std::string, Action> &);
    // template void RvdInsts::getInstHandlers<RV32>(std::map<std::string, Action> &);
    template void RvdInsts::getInstHandlers<RV64>(std::map<std::string, Action> &);

    ActionGroup* RvdInsts::fcvt_d_w_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint32_t rs1_val = READ_INT_REG<RV64>(state, inst->getRs1());
        WRITE_FP_REG<RV64>(state, inst->getRd(), i32_to_f64(rs1_val).v);
        update_csr<RV64>(state);
        return nullptr;
    }

    ActionGroup* RvdInsts::fsub_d_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        softfloat_roundingMode = getRM<RV64>(inst, state);
        const uint64_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        const uint64_t rs2_val = READ_FP_REG<RV64>(state, inst->getRs2());
        WRITE_FP_REG<RV64>(state, inst->getRd(), f64_sub(float64_t{rs1_val}, float64_t{rs2_val}).v);
        update_csr<RV64>(state);
        return nullptr;
    }

    ActionGroup* RvdInsts::fmv_x_d_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint64_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        WRITE_INT_REG<RV64>(state, inst->getRd(), rs1_val);
        return nullptr;
    }

    ActionGroup* RvdInsts::fcvt_wu_d_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint64_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        WRITE_INT_REG<RV64>(state, inst->getRd(),
                            signExtend<uint32_t, uint64_t>(
                                f64_to_ui32(float64_t{rs1_val}, getRM<RV64>(inst, state), true)));
        update_csr<RV64>(state);
        return nullptr;
    }

    ActionGroup* RvdInsts::fnmsub_d_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        softfloat_roundingMode = getRM<RV64>(inst, state);
        const uint64_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        const uint64_t rs2_val = READ_FP_REG<RV64>(state, inst->getRs2());
        const uint64_t rs3_val = READ_FP_REG<RV64>(state, inst->getRs3());
        const uint64_t product = f64_mul(float64_t{rs1_val}, float64_t{rs2_val}).v;
        WRITE_FP_REG<RV64>(state, inst->getRd(),
                           f64_add(float64_t{product ^ (1UL << 63)}, float64_t{rs3_val}).v);
        update_csr<RV64>(state);
        return nullptr;
    }

    ActionGroup* RvdInsts::fle_d_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint64_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        const uint64_t rs2_val = READ_FP_REG<RV64>(state, inst->getRs2());
        WRITE_INT_REG<RV64>(state, inst->getRd(), f64_le(float64_t{rs1_val}, float64_t{rs2_val}));
        update_csr<RV64>(state);
        return nullptr;
    }

    ActionGroup* RvdInsts::fmul_d_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        softfloat_roundingMode = getRM<RV64>(inst, state);
        const uint64_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        const uint64_t rs2_val = READ_FP_REG<RV64>(state, inst->getRs2());
        WRITE_FP_REG<RV64>(state, inst->getRd(), f64_mul(float64_t{rs1_val}, float64_t{rs2_val}).v);
        update_csr<RV64>(state);
        return nullptr;
    }

    ActionGroup* RvdInsts::fsqrt_d_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        softfloat_roundingMode = getRM<RV64>(inst, state);
        const uint64_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        WRITE_FP_REG<RV64>(state, inst->getRd(), f64_sqrt(float64_t{rs1_val}).v);
        update_csr<RV64>(state);
        return nullptr;
    }

    ActionGroup* RvdInsts::fmadd_d_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        softfloat_roundingMode = getRM<RV64>(inst, state);
        const uint64_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        const uint64_t rs2_val = READ_FP_REG<RV64>(state, inst->getRs2());
        const uint64_t rs3_val = READ_FP_REG<RV64>(state, inst->getRs3());
        WRITE_FP_REG<RV64>(
            state, inst->getRd(),
            f64_add(f64_mul(float64_t{rs1_val}, float64_t{rs2_val}), float64_t{rs3_val}).v);
        update_csr<RV64>(state);
        return nullptr;
    }

    ActionGroup* RvdInsts::fnmadd_d_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        softfloat_roundingMode = getRM<RV64>(inst, state);
        const uint64_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        const uint64_t rs2_val = READ_FP_REG<RV64>(state, inst->getRs2());
        const uint64_t rs3_val = READ_FP_REG<RV64>(state, inst->getRs3());
        const uint64_t product = f64_mul(float64_t{rs1_val}, float64_t{rs2_val}).v;
        WRITE_FP_REG<RV64>(state, inst->getRd(),
                           f64_sub(float64_t{product ^ (1UL << 63)}, float64_t{rs3_val}).v);
        update_csr<RV64>(state);
        return nullptr;
    }

    ActionGroup* RvdInsts::fmin_d_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint64_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        const uint64_t rs2_val = READ_FP_REG<RV64>(state, inst->getRs2());
        uint64_t rd_val = f64_le_quiet(float64_t{rs1_val}, float64_t{rs2_val}) ? rs1_val : rs2_val;
        fmax_fmin_nan_zero_check<DP>(rs1_val, rs2_val, rd_val, false);
        WRITE_FP_REG<RV64>(state, inst->getRd(), rd_val);
        update_csr<RV64>(state);
        return nullptr;
    }

    ActionGroup* RvdInsts::fdiv_d_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        softfloat_roundingMode = getRM<RV64>(inst, state);
        const uint64_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        const uint64_t rs2_val = READ_FP_REG<RV64>(state, inst->getRs2());
        WRITE_FP_REG<RV64>(state, inst->getRd(), f64_div(float64_t{rs1_val}, float64_t{rs2_val}).v);
        update_csr<RV64>(state);
        return nullptr;
    }

    ActionGroup* RvdInsts::fld_64_compute_address_handler(atlas::AtlasState* state)
    {
        return compute_address_handler<RV64>(state);
    }

    ActionGroup* RvdInsts::fld_64_handler(atlas::AtlasState* state)
    {
        return float_ls_handler<RV64, DP, true>(state);
    }

    ActionGroup* RvdInsts::fsgnjx_d_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint64_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        const uint64_t rs2_val = READ_FP_REG<RV64>(state, inst->getRs2());
        const uint64_t sign_mask = 1UL << 63;
        WRITE_FP_REG<RV64>(state, inst->getRd(),
                           (rs1_val & ~sign_mask) | ((rs1_val ^ rs2_val) & sign_mask));
        return nullptr;
    }

    ActionGroup* RvdInsts::fmv_d_x_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint64_t rs1_val = READ_INT_REG<RV64>(state, inst->getRs1());
        WRITE_FP_REG<RV64>(state, inst->getRd(), rs1_val);
        return nullptr;
    }

    ActionGroup* RvdInsts::fcvt_w_d_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint64_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        WRITE_INT_REG<RV64>(state, inst->getRd(),
                            signExtend<uint32_t, uint64_t>(
                                f64_to_i32(float64_t{rs1_val}, getRM<RV64>(inst, state), true)));
        update_csr<RV64>(state);
        return nullptr;
    }

    ActionGroup* RvdInsts::fcvt_lu_d_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint64_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        WRITE_INT_REG<RV64>(state, inst->getRd(),
                            f64_to_ui64(float64_t{rs1_val}, getRM<RV64>(inst, state), true));
        update_csr<RV64>(state);
        return nullptr;
    }

    ActionGroup* RvdInsts::fsgnjn_d_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint64_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        const uint64_t rs2_val = READ_FP_REG<RV64>(state, inst->getRs2());
        const uint64_t sign_mask = 1UL << 63;
        WRITE_FP_REG<RV64>(state, inst->getRd(),
                           (rs1_val & ~sign_mask) | ((rs2_val & sign_mask) ^ sign_mask));
        return nullptr;
    }

    ActionGroup* RvdInsts::fclass_d_64_handler(atlas::AtlasState* state)
    {
        state->getCurrentInst()->markUnimplemented();
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_either_extension('D', EXT_ZDINX);
        // require_fp;
        // WRITE_RD(f64_classify(FRS1_D));

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvdInsts::fadd_d_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        softfloat_roundingMode = getRM<RV64>(inst, state);
        const uint64_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        const uint64_t rs2_val = READ_FP_REG<RV64>(state, inst->getRs2());
        WRITE_FP_REG<RV64>(state, inst->getRd(), f64_add(float64_t{rs1_val}, float64_t{rs2_val}).v);
        update_csr<RV64>(state);
        return nullptr;
    }

    ActionGroup* RvdInsts::fmsub_d_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        softfloat_roundingMode = getRM<RV64>(inst, state);
        const uint64_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        const uint64_t rs2_val = READ_FP_REG<RV64>(state, inst->getRs2());
        const uint64_t rs3_val = READ_FP_REG<RV64>(state, inst->getRs3());
        WRITE_FP_REG<RV64>(
            state, inst->getRd(),
            f64_sub(f64_mul(float64_t{rs1_val}, float64_t{rs2_val}), float64_t{rs3_val}).v);
        update_csr<RV64>(state);
        return nullptr;
    }

    ActionGroup* RvdInsts::fcvt_d_s_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint32_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        WRITE_FP_REG<RV64>(state, inst->getRd(), f32_to_f64(float32_t{rs1_val}).v);
        update_csr<RV64>(state);
        return nullptr;
    }

    ActionGroup* RvdInsts::fsd_64_compute_address_handler(atlas::AtlasState* state)
    {
        return compute_address_handler<RV64>(state);
    }

    ActionGroup* RvdInsts::fsd_64_handler(atlas::AtlasState* state)
    {
        return float_ls_handler<RV64, DP, false>(state);
    }

    ActionGroup* RvdInsts::fmax_d_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint64_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        const uint64_t rs2_val = READ_FP_REG<RV64>(state, inst->getRs2());
        uint64_t rd_val = f64_le_quiet(float64_t{rs1_val}, float64_t{rs2_val}) ? rs2_val : rs1_val;
        fmax_fmin_nan_zero_check<DP>(rs1_val, rs2_val, rd_val, true);
        WRITE_FP_REG<RV64>(state, inst->getRd(), rd_val);
        update_csr<RV64>(state);
        return nullptr;
    }

    ActionGroup* RvdInsts::fcvt_s_d_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint64_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        WRITE_FP_REG<RV64>(state, inst->getRd(), f64_to_f32(float64_t{rs1_val}).v);
        update_csr<RV64>(state);
        return nullptr;
    }

    ActionGroup* RvdInsts::fcvt_d_lu_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint64_t rs1_val = READ_INT_REG<RV64>(state, inst->getRs1());
        WRITE_FP_REG<RV64>(state, inst->getRd(), ui64_to_f64(rs1_val).v);
        update_csr<RV64>(state);
        return nullptr;
    }

    ActionGroup* RvdInsts::feq_d_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint64_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        const uint64_t rs2_val = READ_FP_REG<RV64>(state, inst->getRs2());
        WRITE_INT_REG<RV64>(state, inst->getRd(), f64_eq(float64_t{rs1_val}, float64_t{rs2_val}));
        update_csr<RV64>(state);
        return nullptr;
    }

    ActionGroup* RvdInsts::fsgnj_d_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint64_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        const uint64_t rs2_val = READ_FP_REG<RV64>(state, inst->getRs2());
        const uint64_t sign_mask = 1UL << 63;
        WRITE_FP_REG<RV64>(state, inst->getRd(), (rs1_val & ~sign_mask) | (rs2_val & sign_mask));
        return nullptr;
    }

    ActionGroup* RvdInsts::fcvt_d_l_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint64_t rs1_val = READ_INT_REG<RV64>(state, inst->getRs1());
        WRITE_FP_REG<RV64>(state, inst->getRd(), i64_to_f64(rs1_val).v);
        update_csr<RV64>(state);
        return nullptr;
    }

    ActionGroup* RvdInsts::fcvt_d_wu_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint32_t rs1_val = READ_INT_REG<RV64>(state, inst->getRs1());
        WRITE_FP_REG<RV64>(state, inst->getRd(), ui32_to_f64(rs1_val).v);
        update_csr<RV64>(state);
        return nullptr;
    }

    ActionGroup* RvdInsts::flt_d_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint64_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        const uint64_t rs2_val = READ_FP_REG<RV64>(state, inst->getRs2());
        WRITE_INT_REG<RV64>(state, inst->getRd(), f64_lt(float64_t{rs1_val}, float64_t{rs2_val}));
        update_csr<RV64>(state);
        return nullptr;
    }

    ActionGroup* RvdInsts::fcvt_l_d_64_handler(atlas::AtlasState* state)
    {
        const AtlasInstPtr & inst = state->getCurrentInst();
        const uint64_t rs1_val = READ_FP_REG<RV64>(state, inst->getRs1());
        WRITE_INT_REG<RV64>(state, inst->getRd(),
                            f64_to_i64(float64_t{rs1_val}, getRM<RV64>(inst, state), true));
        update_csr<RV64>(state);
        return nullptr;
    }

} // namespace atlas
