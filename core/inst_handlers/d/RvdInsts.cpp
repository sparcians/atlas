#include "core/inst_handlers/d/RvdInsts.hpp"
#include "include/ActionTags.hpp"
#include "core/ActionGroup.hpp"

namespace atlas
{
    template <typename XLEN>
    void RvdInsts::getInstComputeAddressHandlers(std::map<std::string, Action> & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same<XLEN, RV32>::value);
        if constexpr (std::is_same_v<XLEN, RV64>)
        {
            inst_handlers.emplace(
                "fld",
                atlas::Action::createAction<&RvdInsts::fld_64_compute_address_handler, RvdInsts>(
                    nullptr, "fld", ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "flw",
                atlas::Action::createAction<&RvdInsts::flw_64_compute_address_handler, RvdInsts>(
                    nullptr, "flw", ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "fsd",
                atlas::Action::createAction<&RvdInsts::fsd_64_compute_address_handler, RvdInsts>(
                    nullptr, "fsd", ActionTags::COMPUTE_ADDR_TAG));
            inst_handlers.emplace(
                "fsw",
                atlas::Action::createAction<&RvdInsts::fsw_64_compute_address_handler, RvdInsts>(
                    nullptr, "fsw", ActionTags::COMPUTE_ADDR_TAG));
        }
        else if constexpr (std::is_same_v<XLEN, RV32>)
        {
            sparta_assert(false, "RV32 is not supported yet!");
        }
    }

    template <typename XLEN>
    void RvdInsts::getInstHandlers(std::map<std::string, Action> & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same<XLEN, RV32>::value);
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
            inst_handlers.emplace("flw",
                                  atlas::Action::createAction<&RvdInsts::flw_64_handler, RvdInsts>(
                                      nullptr, "flw", ActionTags::EXECUTE_TAG));
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
                "fmv.w.x", atlas::Action::createAction<&RvdInsts::fmv_w_x_64_handler, RvdInsts>(
                               nullptr, "fmv_w_x", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "fmv.x.d", atlas::Action::createAction<&RvdInsts::fmv_x_d_64_handler, RvdInsts>(
                               nullptr, "fmv_x_d", ActionTags::EXECUTE_TAG));
            inst_handlers.emplace(
                "fmv.x.w", atlas::Action::createAction<&RvdInsts::fmv_x_w_64_handler, RvdInsts>(
                               nullptr, "fmv_x_w", ActionTags::EXECUTE_TAG));
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
            inst_handlers.emplace("fsw",
                                  atlas::Action::createAction<&RvdInsts::fsw_64_handler, RvdInsts>(
                                      nullptr, "fsw", ActionTags::EXECUTE_TAG));
        }
        else if constexpr (std::is_same_v<XLEN, RV32>)
        {
            sparta_assert(false, "RV32 is not supported yet!");
        }
    }

    template void RvdInsts::getInstComputeAddressHandlers<RV32>(std::map<std::string, Action> &);
    template void RvdInsts::getInstComputeAddressHandlers<RV64>(std::map<std::string, Action> &);
    template void RvdInsts::getInstHandlers<RV32>(std::map<std::string, Action> &);
    template void RvdInsts::getInstHandlers<RV64>(std::map<std::string, Action> &);

    ActionGroup* RvdInsts::fcvt_d_w_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_either_extension('D', EXT_ZDINX);
        // require_fp;
        // softfloat_roundingMode = RM;
        // WRITE_FRD_D(i32_to_f64((int32_t)RS1));
        // set_fp_exceptions;

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvdInsts::fsub_d_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_either_extension('D', EXT_ZDINX);
        // require_fp;
        // softfloat_roundingMode = RM;
        // WRITE_FRD_D(f64_sub(FRS1_D, FRS2_D));
        // set_fp_exceptions;

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvdInsts::fmv_x_d_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_extension('D');
        // require_rv64;
        // require_fp;
        // WRITE_RD(FRS1.v[0]);

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvdInsts::fcvt_wu_d_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_either_extension('D', EXT_ZDINX);
        // require_fp;
        // softfloat_roundingMode = RM;
        // WRITE_RD(sext32(f64_to_ui32(FRS1_D, RM, true)));
        // set_fp_exceptions;

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvdInsts::fnmsub_d_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_either_extension('D', EXT_ZDINX);
        // require_fp;
        // softfloat_roundingMode = RM;
        // WRITE_FRD_D(f64_mulAdd(f64(FRS1_D.v ^ F64_SIGN), FRS2_D, FRS3_D));
        // set_fp_exceptions;

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvdInsts::fle_d_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_either_extension('D', EXT_ZDINX);
        // require_fp;
        // WRITE_RD(f64_le(FRS1_D, FRS2_D));
        // set_fp_exceptions;

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvdInsts::fmul_d_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_either_extension('D', EXT_ZDINX);
        // require_fp;
        // softfloat_roundingMode = RM;
        // WRITE_FRD_D(f64_mul(FRS1_D, FRS2_D));
        // set_fp_exceptions;

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvdInsts::fsqrt_d_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_either_extension('D', EXT_ZDINX);
        // require_fp;
        // softfloat_roundingMode = RM;
        // WRITE_FRD_D(f64_sqrt(FRS1_D));
        // set_fp_exceptions;

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvdInsts::fmadd_d_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_either_extension('D', EXT_ZDINX);
        // require_fp;
        // softfloat_roundingMode = RM;
        // WRITE_FRD_D(f64_mulAdd(FRS1_D, FRS2_D, FRS3_D));
        // set_fp_exceptions;

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvdInsts::fnmadd_d_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_either_extension('D', EXT_ZDINX);
        // require_fp;
        // softfloat_roundingMode = RM;
        // WRITE_FRD_D(f64_mulAdd(f64(FRS1_D.v ^ F64_SIGN), FRS2_D, f64(FRS3_D.v ^ F64_SIGN)));
        // set_fp_exceptions;

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvdInsts::fmv_w_x_64_handler(atlas::AtlasState* state)
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

    ActionGroup* RvdInsts::fmin_d_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_either_extension('D', EXT_ZDINX);
        // require_fp;
        // bool less = f64_lt_quiet(FRS1_D, FRS2_D) ||
        //                         (f64_eq(FRS1_D, FRS2_D) && (FRS1_D.v & F64_SIGN));
        // if (isNaNF64UI(FRS1_D.v) && isNaNF64UI(FRS2_D.v))
        //     WRITE_FRD_D(f64(defaultNaNF64UI));
        // else
        //     WRITE_FRD_D((less || isNaNF64UI(FRS2_D.v) ? FRS1_D : FRS2_D));
        // set_fp_exceptions;

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvdInsts::fdiv_d_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_either_extension('D', EXT_ZDINX);
        // require_fp;
        // softfloat_roundingMode = RM;
        // WRITE_FRD_D(f64_div(FRS1_D, FRS2_D));
        // set_fp_exceptions;

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvdInsts::fld_64_compute_address_handler(atlas::AtlasState* state)
    {
        (void)state;
        return nullptr;
    }

    ActionGroup* RvdInsts::fld_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_extension('D');
        // require_fp;
        // WRITE_FRD(f64(MMU.load<uint64_t>(RS1 + insn.i_imm())));

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvdInsts::fsw_64_compute_address_handler(atlas::AtlasState* state)
    {
        (void)state;
        return nullptr;
    }

    ActionGroup* RvdInsts::fsw_64_handler(atlas::AtlasState* state)
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

    ActionGroup* RvdInsts::fmv_x_w_64_handler(atlas::AtlasState* state)
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

    ActionGroup* RvdInsts::fsgnjx_d_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_either_extension('D', EXT_ZDINX);
        // require_fp;
        // WRITE_FRD_D(fsgnj64(freg(FRS1_D), freg(FRS2_D), false, true));

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvdInsts::flw_64_compute_address_handler(atlas::AtlasState* state)
    {
        (void)state;
        return nullptr;
    }

    ActionGroup* RvdInsts::flw_64_handler(atlas::AtlasState* state)
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

    ActionGroup* RvdInsts::fmv_d_x_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_extension('D');
        // require_rv64;
        // require_fp;
        // WRITE_FRD(f64(RS1));

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvdInsts::fcvt_w_d_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_either_extension('D', EXT_ZDINX);
        // require_fp;
        // softfloat_roundingMode = RM;
        // WRITE_RD(sext32(f64_to_i32(FRS1_D, RM, true)));
        // set_fp_exceptions;

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvdInsts::fcvt_lu_d_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_either_extension('D', EXT_ZDINX);
        // require_rv64;
        // require_fp;
        // softfloat_roundingMode = RM;
        // WRITE_RD(f64_to_ui64(FRS1_D, RM, true));
        // set_fp_exceptions;

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvdInsts::fsgnjn_d_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_either_extension('D', EXT_ZDINX);
        // require_fp;
        // WRITE_FRD_D(fsgnj64(freg(FRS1_D), freg(FRS2_D), true, false));

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvdInsts::fclass_d_64_handler(atlas::AtlasState* state)
    {
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
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_either_extension('D', EXT_ZDINX);
        // require_fp;
        // softfloat_roundingMode = RM;
        // WRITE_FRD_D(f64_add(FRS1_D, FRS2_D));
        // set_fp_exceptions;

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvdInsts::fmsub_d_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_either_extension('D', EXT_ZDINX);
        // require_fp;
        // softfloat_roundingMode = RM;
        // WRITE_FRD_D(f64_mulAdd(FRS1_D, FRS2_D, f64(FRS3_D.v ^ F64_SIGN)));
        // set_fp_exceptions;

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvdInsts::fcvt_d_s_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_either_extension('D', EXT_ZDINX);
        // require_fp;
        // softfloat_roundingMode = RM;
        // WRITE_FRD_D(f32_to_f64(FRS1_F));
        // set_fp_exceptions;

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvdInsts::fsd_64_compute_address_handler(atlas::AtlasState* state)
    {
        (void)state;
        return nullptr;
    }

    ActionGroup* RvdInsts::fsd_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_extension('D');
        // require_fp;
        // MMU.store<uint64_t>(RS1 + insn.s_imm(), FRS2.v[0]);

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvdInsts::fmax_d_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_either_extension('D', EXT_ZDINX);
        // require_fp;
        // bool greater = f64_lt_quiet(FRS2_D, FRS1_D) ||
        //                             (f64_eq(FRS2_D, FRS1_D) && (FRS2_D.v & F64_SIGN));
        // if (isNaNF64UI(FRS1_D.v) && isNaNF64UI(FRS2_D.v))
        //     WRITE_FRD_D(f64(defaultNaNF64UI));
        // else
        //     WRITE_FRD_D((greater || isNaNF64UI(FRS2_D.v) ? FRS1_D : FRS2_D));
        // set_fp_exceptions;

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvdInsts::fcvt_s_d_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_either_extension('D', EXT_ZDINX);
        // require_fp;
        // softfloat_roundingMode = RM;
        // WRITE_FRD_F(f64_to_f32(FRS1_D));
        // set_fp_exceptions;

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvdInsts::fcvt_d_lu_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_either_extension('D', EXT_ZDINX);
        // require_rv64;
        // require_fp;
        // softfloat_roundingMode = RM;
        // WRITE_FRD_D(ui64_to_f64(RS1));
        // set_fp_exceptions;

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvdInsts::feq_d_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_either_extension('D', EXT_ZDINX);
        // require_fp;
        // WRITE_RD(f64_eq(FRS1_D, FRS2_D));
        // set_fp_exceptions;

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvdInsts::fsgnj_d_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_either_extension('D', EXT_ZDINX);
        // require_fp;
        // WRITE_FRD_D(fsgnj64(freg(FRS1_D), freg(FRS2_D), false, false));

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvdInsts::fcvt_d_l_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_either_extension('D', EXT_ZDINX);
        // require_rv64;
        // require_fp;
        // softfloat_roundingMode = RM;
        // WRITE_FRD_D(i64_to_f64(RS1));
        // set_fp_exceptions;

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvdInsts::fcvt_d_wu_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_either_extension('D', EXT_ZDINX);
        // require_fp;
        // softfloat_roundingMode = RM;
        // WRITE_FRD_D(ui32_to_f64((uint32_t)RS1));
        // set_fp_exceptions;

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvdInsts::flt_d_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_either_extension('D', EXT_ZDINX);
        // require_fp;
        // WRITE_RD(f64_lt(FRS1_D, FRS2_D));
        // set_fp_exceptions;

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

    ActionGroup* RvdInsts::fcvt_l_d_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // require_either_extension('D', EXT_ZDINX);
        // require_rv64;
        // require_fp;
        // softfloat_roundingMode = RM;
        // WRITE_RD(f64_to_i64(FRS1_D, RM, true));
        // set_fp_exceptions;

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

} // namespace atlas
