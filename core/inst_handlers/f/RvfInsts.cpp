#include "core/inst_handlers/f/RvfInsts.hpp"
#include "include/ActionTags.hpp"
#include "core/ActionGroup.hpp"

namespace atlas
{
    template <typename XLEN>
    void RvfInsts::getInstComputeAddressHandlers(std::map<std::string, Action> & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same<XLEN, RV32>::value);
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
            sparta_assert(false, "RV32 is not supported yet!");
        }
    }

    template <typename XLEN>
    void RvfInsts::getInstHandlers(std::map<std::string, Action> & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same<XLEN, RV32>::value);
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

    template void RvfInsts::getInstComputeAddressHandlers<RV32>(std::map<std::string, Action> &);
    template void RvfInsts::getInstComputeAddressHandlers<RV64>(std::map<std::string, Action> &);
    template void RvfInsts::getInstHandlers<RV32>(std::map<std::string, Action> &);
    template void RvfInsts::getInstHandlers<RV64>(std::map<std::string, Action> &);

    ActionGroup* RvfInsts::fsqrt_s_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_either_extension('F', EXT_ZFINX);
        // require_fp;
        // softfloat_roundingMode = RM;
        // WRITE_FRD_F(f32_sqrt(FRS1_F));
        // set_fp_exceptions;

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvfInsts::fsub_s_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_either_extension('F', EXT_ZFINX);
        // require_fp;
        // softfloat_roundingMode = RM;
        // WRITE_FRD_F(f32_sub(FRS1_F, FRS2_F));
        // set_fp_exceptions;

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvfInsts::fnmsub_s_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_either_extension('F', EXT_ZFINX);
        // require_fp;
        // softfloat_roundingMode = RM;
        // WRITE_FRD_F(f32_mulAdd(f32(FRS1_F.v ^ F32_SIGN), FRS2_F, FRS3_F));
        // set_fp_exceptions;

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvfInsts::feq_s_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_either_extension('F', EXT_ZFINX);
        // require_fp;
        // WRITE_RD(f32_eq(FRS1_F, FRS2_F));
        // set_fp_exceptions;

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvfInsts::fclass_s_64_handler(atlas::AtlasState* state)
    {
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
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_either_extension('F', EXT_ZFINX);
        // require_fp;
        // softfloat_roundingMode = RM;
        // WRITE_FRD_F(f32_mulAdd(FRS1_F, FRS2_F, f32(FRS3_F.v ^ F32_SIGN)));
        // set_fp_exceptions;

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvfInsts::fmin_s_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_either_extension('F', EXT_ZFINX);
        // require_fp;
        // bool less = f32_lt_quiet(FRS1_F, FRS2_F) ||
        //                         (f32_eq(FRS1_F, FRS2_F) && (FRS1_F.v & F32_SIGN));
        // if (isNaNF32UI(FRS1_F.v) && isNaNF32UI(FRS2_F.v))
        //     WRITE_FRD_F(f32(defaultNaNF32UI));
        // else
        //     WRITE_FRD_F((less || isNaNF32UI(FRS2_F.v) ? FRS1_F : FRS2_F));
        // set_fp_exceptions;

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvfInsts::fmv_w_x_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_extension('F');
        // require_fp;
        // WRITE_FRD(f32(RS1));

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvfInsts::fcvt_lu_s_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_either_extension('F', EXT_ZFINX);
        // require_rv64;
        // require_fp;
        // softfloat_roundingMode = RM;
        // WRITE_RD(f32_to_ui64(FRS1_F, RM, true));
        // set_fp_exceptions;

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvfInsts::fcvt_s_w_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_either_extension('F', EXT_ZFINX);
        // require_fp;
        // softfloat_roundingMode = RM;
        // WRITE_FRD_F(i32_to_f32((int32_t)RS1));
        // set_fp_exceptions;

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvfInsts::fnmadd_s_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_either_extension('F', EXT_ZFINX);
        // require_fp;
        // softfloat_roundingMode = RM;
        // WRITE_FRD_F(f32_mulAdd(f32(FRS1_F.v ^ F32_SIGN), FRS2_F, f32(FRS3_F.v ^ F32_SIGN)));
        // set_fp_exceptions;

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvfInsts::fcvt_s_l_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_either_extension('F', EXT_ZFINX);
        // require_rv64;
        // require_fp;
        // softfloat_roundingMode = RM;
        // WRITE_FRD_F(i64_to_f32(RS1));
        // set_fp_exceptions;

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvfInsts::fadd_s_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_either_extension('F', EXT_ZFINX);
        // require_fp;
        // softfloat_roundingMode = RM;
        // WRITE_FRD_F(f32_add(FRS1_F, FRS2_F));
        // set_fp_exceptions;

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvfInsts::fsw_64_compute_address_handler(atlas::AtlasState* state)
    {
        (void)state;
        return nullptr;
    }

    ActionGroup* RvfInsts::fsw_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_extension('F');
        // require_fp;
        // MMU.store<uint32_t>(RS1 + insn.s_imm(), FRS2.v[0]);

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvfInsts::fmv_x_w_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_extension('F');
        // require_fp;
        // WRITE_RD(sext32(FRS1.v[0]));

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvfInsts::fmax_s_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_either_extension('F', EXT_ZFINX);
        // require_fp;
        // bool greater = f32_lt_quiet(FRS2_F, FRS1_F) ||
        //                              (f32_eq(FRS2_F, FRS1_F) && (FRS2_F.v & F32_SIGN));
        // if (isNaNF32UI(FRS1_F.v) && isNaNF32UI(FRS2_F.v))
        //     WRITE_FRD_F(f32(defaultNaNF32UI));
        // else
        //     WRITE_FRD_F((greater || isNaNF32UI(FRS2_F.v) ? FRS1_F : FRS2_F));
        // set_fp_exceptions;

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvfInsts::fsgnjx_s_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_either_extension('F', EXT_ZFINX);
        // require_fp;
        // WRITE_FRD_F(fsgnj32(freg(FRS1_F), freg(FRS2_F), false, true));

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvfInsts::flw_64_compute_address_handler(atlas::AtlasState* state)
    {
        (void)state;
        return nullptr;
    }

    ActionGroup* RvfInsts::flw_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_extension('F');
        // require_fp;
        // WRITE_FRD(f32(MMU.load<uint32_t>(RS1 + insn.i_imm())));

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvfInsts::fmadd_s_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_either_extension('F', EXT_ZFINX);
        // require_fp;
        // softfloat_roundingMode = RM;
        // WRITE_FRD_F(f32_mulAdd(FRS1_F, FRS2_F, FRS3_F));
        // set_fp_exceptions;

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvfInsts::fmul_s_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_either_extension('F', EXT_ZFINX);
        // require_fp;
        // softfloat_roundingMode = RM;
        // WRITE_FRD_F(f32_mul(FRS1_F, FRS2_F));
        // set_fp_exceptions;

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvfInsts::flt_s_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_either_extension('F', EXT_ZFINX);
        // require_fp;
        // WRITE_RD(f32_lt(FRS1_F, FRS2_F));
        // set_fp_exceptions;

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvfInsts::fcvt_w_s_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_either_extension('F', EXT_ZFINX);
        // require_fp;
        // softfloat_roundingMode = RM;
        // WRITE_RD(sext32(f32_to_i32(FRS1_F, RM, true)));
        // set_fp_exceptions;

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvfInsts::fcvt_l_s_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_either_extension('F', EXT_ZFINX);
        // require_rv64;
        // require_fp;
        // softfloat_roundingMode = RM;
        // WRITE_RD(f32_to_i64(FRS1_F, RM, true));
        // set_fp_exceptions;

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvfInsts::fsgnjn_s_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_either_extension('F', EXT_ZFINX);
        // require_fp;
        // WRITE_FRD_F(fsgnj32(freg(FRS1_F), freg(FRS2_F), true, false));

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvfInsts::fcvt_s_lu_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_either_extension('F', EXT_ZFINX);
        // require_rv64;
        // require_fp;
        // softfloat_roundingMode = RM;
        // WRITE_FRD_F(ui64_to_f32(RS1));
        // set_fp_exceptions;

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvfInsts::fcvt_wu_s_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_either_extension('F', EXT_ZFINX);
        // require_fp;
        // softfloat_roundingMode = RM;
        // WRITE_RD(sext32(f32_to_ui32(FRS1_F, RM, true)));
        // set_fp_exceptions;

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvfInsts::fdiv_s_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_either_extension('F', EXT_ZFINX);
        // require_fp;
        // softfloat_roundingMode = RM;
        // WRITE_FRD_F(f32_div(FRS1_F, FRS2_F));
        // set_fp_exceptions;

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvfInsts::fsgnj_s_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_either_extension('F', EXT_ZFINX);
        // require_fp;
        // WRITE_FRD_F(fsgnj32(freg(FRS1_F), freg(FRS2_F), false, false));

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvfInsts::fcvt_s_wu_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_either_extension('F', EXT_ZFINX);
        // require_fp;
        // softfloat_roundingMode = RM;
        // WRITE_FRD_F(ui32_to_f32((uint32_t)RS1));
        // set_fp_exceptions;

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvfInsts::fle_s_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_either_extension('F', EXT_ZFINX);
        // require_fp;
        // WRITE_RD(f32_le(FRS1_F, FRS2_F));
        // set_fp_exceptions;

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

} // namespace atlas
