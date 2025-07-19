#include <limits>
#include <type_traits>

#include "core/inst_handlers/v/RvvFloatInsts.hpp"
#include "core/inst_handlers/finsts_helpers.hpp"
#include "core/PegasusState.hpp"
#include "core/ActionGroup.hpp"
#include "core/VecElements.hpp"
#include "include/ActionTags.hpp"
#include "include/PegasusUtils.hpp"

namespace pegasus
{
    template <typename XLEN>
    void RvvFloatInsts::getInstHandlers(std::map<std::string, Action> & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);

        // move operation

        inst_handlers.emplace(
            "vfmv.v.f",
            pegasus::Action::createAction<&RvvFloatInsts::vfmvHandler_<XLEN>, RvvFloatInsts>(
                nullptr, "vfmv.v.f", ActionTags::EXECUTE_TAG));

        // merge operation

        inst_handlers.emplace(
            "vfmerge.vfm",
            pegasus::Action::createAction<&RvvFloatInsts::vfmergeHandler_<XLEN>, RvvFloatInsts>(
                nullptr, "vfmerge.vfm", ActionTags::EXECUTE_TAG));

        // unary operations

        inst_handlers.emplace(
            "vfsqrt.v",
            pegasus::Action::createAction<
                &RvvFloatInsts::vfUnaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::V},
                    FloatFuncs{f16_sqrt, f32_sqrt, f64_sqrt}>,
                RvvFloatInsts>(nullptr, "vfsqrt.v", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vfcvt.xu.f.v",
            pegasus::Action::createAction<
                &RvvFloatInsts::vfFloatToIntHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::V},
                    FloatFuncs{nullptr, f32_to_ui32, f64_to_ui64}>,
                RvvFloatInsts>(nullptr, "vfcvt.xu.f.v", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfcvt.x.f.v",
            pegasus::Action::createAction<
                &RvvFloatInsts::vfFloatToIntHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::V},
                    FloatFuncs{nullptr, f32_to_i32, f64_to_i64}>,
                RvvFloatInsts>(nullptr, "vfcvt.x.f.v", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfcvt.f.xu.v",
            pegasus::Action::createAction<
                &RvvFloatInsts::vfUnaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::V},
                    FloatFuncs{nullptr, ui32_to_f32, ui64_to_f64}>,
                RvvFloatInsts>(nullptr, "vfcvt.f.xu.v", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfcvt.f.x.v",
            pegasus::Action::createAction<
                &RvvFloatInsts::vfUnaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::V},
                    FloatFuncs{nullptr, i32_to_f32, i64_to_f64}>,
                RvvFloatInsts>(nullptr, "vfcvt.f.x.v", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vfwcvt.xu.f.v",
            pegasus::Action::createAction<
                &RvvFloatInsts::vfFloatToIntHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::W, OperandMode::Mode::V},
                    FloatFuncs{f16_to_ui32, f32_to_ui64, nullptr}>,
                RvvFloatInsts>(nullptr, "vfwcvt.xu.f.v", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfwcvt.x.f.v",
            pegasus::Action::createAction<
                &RvvFloatInsts::vfFloatToIntHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::W, OperandMode::Mode::V},
                    FloatFuncs{f16_to_i32, f32_to_i64, nullptr}>,
                RvvFloatInsts>(nullptr, "vfwcvt.x.f.v", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfwcvt.f.xu.v",
            pegasus::Action::createAction<
                &RvvFloatInsts::vfUnaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::W, OperandMode::Mode::V},
                    FloatFuncs{nullptr, ui32_to_f64, nullptr}>,
                RvvFloatInsts>(nullptr, "vfwcvt.f.xu.v", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfwcvt.f.x.v",
            pegasus::Action::createAction<
                &RvvFloatInsts::vfUnaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::W, OperandMode::Mode::V},
                    FloatFuncs{nullptr, i32_to_f64, nullptr}>,
                RvvFloatInsts>(nullptr, "vfwcvt.f.x.v", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfwcvt.f.f.v",
            pegasus::Action::createAction<
                &RvvFloatInsts::vfUnaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::W, OperandMode::Mode::V},
                    FloatFuncs{f16_to_f32, f32_to_f64, nullptr}>,
                RvvFloatInsts>(nullptr, "vfwcvt.f.x.v", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vfncvt.xu.f.w",
            pegasus::Action::createAction<
                &RvvFloatInsts::vfFloatToIntHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::W, OperandMode::Mode::V},
                    FloatFuncs{nullptr, f64_to_ui32, nullptr}>,
                RvvFloatInsts>(nullptr, "vfncvt.xu.f.w", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfncvt.x.f.w",
            pegasus::Action::createAction<
                &RvvFloatInsts::vfFloatToIntHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::W, OperandMode::Mode::V},
                    FloatFuncs{nullptr, f64_to_i32, nullptr}>,
                RvvFloatInsts>(nullptr, "vfncvt.x.f.w", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfncvt.f.xu.w",
            pegasus::Action::createAction<
                &RvvFloatInsts::vfUnaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::W, OperandMode::Mode::V},
                    FloatFuncs{ui32_to_f16, ui64_to_f32, nullptr}>,
                RvvFloatInsts>(nullptr, "vfncvt.f.xu.w", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfncvt.f.x.w",
            pegasus::Action::createAction<
                &RvvFloatInsts::vfUnaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::W, OperandMode::Mode::V},
                    FloatFuncs{i32_to_f16, i64_to_f32, nullptr}>,
                RvvFloatInsts>(nullptr, "vfncvt.f.x.w", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfclass.v",
            pegasus::Action::createAction<
                &RvvFloatInsts::vfUnaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::V},
                    FloatFuncs{fclass<uint16_t>, fclass<uint32_t>, fclass<uint64_t>}>,
                RvvFloatInsts>(nullptr, "vfclass.v", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vfncvt.f.f.w",
            pegasus::Action::createAction<
                &RvvFloatInsts::vfUnaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::W, OperandMode::Mode::V},
                    FloatFuncs{f32_to_f16, f64_to_f32, nullptr}>,
                RvvFloatInsts>(nullptr, "vfncvt.f.f.w", ActionTags::EXECUTE_TAG));

        // binary operations

        inst_handlers.emplace(
            "vfadd.vv",
            pegasus::Action::createAction<
                &RvvFloatInsts::vfBinaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::V},
                    FloatFuncs{f16_add, f32_add, f64_add}>,
                RvvFloatInsts>(nullptr, "vfadd.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfadd.vf",
            pegasus::Action::createAction<
                &RvvFloatInsts::vfBinaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::F},
                    FloatFuncs{f16_add, f32_add, f64_add}>,
                RvvFloatInsts>(nullptr, "vfadd.vf", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfsub.vv",
            pegasus::Action::createAction<
                &RvvFloatInsts::vfBinaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::V},
                    FloatFuncs{f16_sub, f32_sub, f64_sub}>,
                RvvFloatInsts>(nullptr, "vfsub.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfsub.vf",
            pegasus::Action::createAction<
                &RvvFloatInsts::vfBinaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::F},
                    FloatFuncs{f16_sub, f32_sub, f64_sub}>,
                RvvFloatInsts>(nullptr, "vfsub.vf", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfrsub.vf",
            pegasus::Action::createAction<
                &RvvFloatInsts::vfrBinaryHandler_<XLEN, FloatFuncs{f16_sub, f32_sub, f64_sub}>,
                RvvFloatInsts>(nullptr, "vfrsub.vf", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfmul.vv",
            pegasus::Action::createAction<
                &RvvFloatInsts::vfBinaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::V},
                    FloatFuncs{f16_mul, f32_mul, f64_mul}>,
                RvvFloatInsts>(nullptr, "vfmul.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfmul.vf",
            pegasus::Action::createAction<
                &RvvFloatInsts::vfBinaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::F},
                    FloatFuncs{f16_mul, f32_mul, f64_mul}>,
                RvvFloatInsts>(nullptr, "vfmul.vf", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfdiv.vv",
            pegasus::Action::createAction<
                &RvvFloatInsts::vfBinaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::V},
                    FloatFuncs{f16_div, f32_div, f64_div}>,
                RvvFloatInsts>(nullptr, "vfdiv.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfdiv.vf",
            pegasus::Action::createAction<
                &RvvFloatInsts::vfBinaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::F},
                    FloatFuncs{f16_div, f32_div, f64_div}>,
                RvvFloatInsts>(nullptr, "vfdiv.vf", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfrdiv.vf",
            pegasus::Action::createAction<
                &RvvFloatInsts::vfrBinaryHandler_<XLEN, FloatFuncs{f16_div, f32_div, f64_div}>,
                RvvFloatInsts>(nullptr, "vfrdiv.vf", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vfwadd.vv",
            pegasus::Action::createAction<
                &RvvFloatInsts::vfBinaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::W, OperandMode::Mode::V, OperandMode::Mode::V},
                    FloatFuncs{f16_add, f32_add, f64_add}>,
                RvvFloatInsts>(nullptr, "vfwadd.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfwadd.vf",
            pegasus::Action::createAction<
                &RvvFloatInsts::vfBinaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::W, OperandMode::Mode::V, OperandMode::Mode::F},
                    FloatFuncs{f16_add, f32_add, f64_add}>,
                RvvFloatInsts>(nullptr, "vfwadd.vf", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfwsub.vv",
            pegasus::Action::createAction<
                &RvvFloatInsts::vfBinaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::W, OperandMode::Mode::V, OperandMode::Mode::V},
                    FloatFuncs{f16_sub, f32_sub, f64_sub}>,
                RvvFloatInsts>(nullptr, "vfwsub.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfwsub.vf",
            pegasus::Action::createAction<
                &RvvFloatInsts::vfBinaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::W, OperandMode::Mode::V, OperandMode::Mode::F},
                    FloatFuncs{f16_sub, f32_sub, f64_sub}>,
                RvvFloatInsts>(nullptr, "vfwsub.vf", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfwmul.vv",
            pegasus::Action::createAction<
                &RvvFloatInsts::vfBinaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::W, OperandMode::Mode::V, OperandMode::Mode::V},
                    FloatFuncs{f16_mul, f32_mul, f64_mul}>,
                RvvFloatInsts>(nullptr, "vfwmul.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfwmul.vf",
            pegasus::Action::createAction<
                &RvvFloatInsts::vfBinaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::W, OperandMode::Mode::V, OperandMode::Mode::F},
                    FloatFuncs{f16_mul, f32_mul, f64_mul}>,
                RvvFloatInsts>(nullptr, "vfwmul.vf", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfwadd.wv",
            pegasus::Action::createAction<
                &RvvFloatInsts::vfBinaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::W, OperandMode::Mode::W, OperandMode::Mode::V},
                    FloatFuncs{f16_add, f32_add, f64_add}>,
                RvvFloatInsts>(nullptr, "vfwadd.wv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfwadd.wf",
            pegasus::Action::createAction<
                &RvvFloatInsts::vfBinaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::W, OperandMode::Mode::W, OperandMode::Mode::F},
                    FloatFuncs{f16_add, f32_add, f64_add}>,
                RvvFloatInsts>(nullptr, "vfwadd.wf", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfwsub.wv",
            pegasus::Action::createAction<
                &RvvFloatInsts::vfBinaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::W, OperandMode::Mode::W, OperandMode::Mode::V},
                    FloatFuncs{f16_sub, f32_sub, f64_sub}>,
                RvvFloatInsts>(nullptr, "vfwsub.wv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfwsub.wf",
            pegasus::Action::createAction<
                &RvvFloatInsts::vfBinaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::W, OperandMode::Mode::W, OperandMode::Mode::F},
                    FloatFuncs{f16_sub, f32_sub, f64_sub}>,
                RvvFloatInsts>(nullptr, "vfwsub.wf", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vfsgnj.vv",
            pegasus::Action::createAction<
                &RvvFloatInsts::vfBinaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::V},
                    FloatFuncs{fsgnj<float16_t>, fsgnj<float32_t>, fsgnj<float64_t>}>,
                RvvFloatInsts>(nullptr, "vfsgnj.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfsgnj.vf",
            pegasus::Action::createAction<
                &RvvFloatInsts::vfBinaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::F},
                    FloatFuncs{fsgnj<float16_t>, fsgnj<float32_t>, fsgnj<float64_t>}>,
                RvvFloatInsts>(nullptr, "vfsgnj.vf", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfsgnjn.vv",
            pegasus::Action::createAction<
                &RvvFloatInsts::vfBinaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::V},
                    FloatFuncs{fsgnjn<float16_t>, fsgnjn<float32_t>, fsgnjn<float64_t>}>,
                RvvFloatInsts>(nullptr, "vfsgnjn.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfsgnjn.vf",
            pegasus::Action::createAction<
                &RvvFloatInsts::vfBinaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::F},
                    FloatFuncs{fsgnjn<float16_t>, fsgnjn<float32_t>, fsgnjn<float64_t>}>,
                RvvFloatInsts>(nullptr, "vfsgnjn.vf", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfsgnjx.vv",
            pegasus::Action::createAction<
                &RvvFloatInsts::vfBinaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::V},
                    FloatFuncs{fsgnjx<float16_t>, fsgnjx<float32_t>, fsgnjx<float64_t>}>,
                RvvFloatInsts>(nullptr, "vfsgnjx.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfsgnjx.vf",
            pegasus::Action::createAction<
                &RvvFloatInsts::vfBinaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::F},
                    FloatFuncs{fsgnjx<float16_t>, fsgnjx<float32_t>, fsgnjx<float64_t>}>,
                RvvFloatInsts>(nullptr, "vfsgnjx.vf", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vmfeq.vv",
            pegasus::Action::createAction<
                &RvvFloatInsts::vmfBinaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::V},
                    FloatFuncs{f16_eq, f32_eq, f64_eq}>,
                RvvFloatInsts>(nullptr, "vmfeq.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmfeq.vf",
            pegasus::Action::createAction<
                &RvvFloatInsts::vmfBinaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::F},
                    FloatFuncs{f16_eq, f32_eq, f64_eq}>,
                RvvFloatInsts>(nullptr, "vmfeq.vf", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmfne.vv",
            pegasus::Action::createAction<
                &RvvFloatInsts::vmfBinaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::V},
                    FloatFuncs{fne<float16_t>, fne<float32_t>, fne<float64_t>}>,
                RvvFloatInsts>(nullptr, "vmfne.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmfne.vf",
            pegasus::Action::createAction<
                &RvvFloatInsts::vmfBinaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::F},
                    FloatFuncs{fne<float16_t>, fne<float32_t>, fne<float64_t>}>,
                RvvFloatInsts>(nullptr, "vmfne.vf", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmflt.vv",
            pegasus::Action::createAction<
                &RvvFloatInsts::vmfBinaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::V},
                    FloatFuncs{f16_lt, f32_lt, f64_lt}>,
                RvvFloatInsts>(nullptr, "vmflt.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmflt.vf",
            pegasus::Action::createAction<
                &RvvFloatInsts::vmfBinaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::F},
                    FloatFuncs{f16_lt, f32_lt, f64_lt}>,
                RvvFloatInsts>(nullptr, "vmflt.vf", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmfle.vv",
            pegasus::Action::createAction<
                &RvvFloatInsts::vmfBinaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::V},
                    FloatFuncs{f16_le, f32_le, f64_le}>,
                RvvFloatInsts>(nullptr, "vmfle.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmfle.vf",
            pegasus::Action::createAction<
                &RvvFloatInsts::vmfBinaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::F},
                    FloatFuncs{f16_le, f32_le, f64_le}>,
                RvvFloatInsts>(nullptr, "vmfle.vf", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmfgt.vf",
            pegasus::Action::createAction<
                &RvvFloatInsts::vmfBinaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::F},
                    FloatFuncs{fgt<float16_t>, fgt<float32_t>, fgt<float64_t>}>,
                RvvFloatInsts>(nullptr, "vmfgt.vf", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmfge.vf",
            pegasus::Action::createAction<
                &RvvFloatInsts::vmfBinaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::F},
                    FloatFuncs{fge<float16_t>, fge<float32_t>, fge<float64_t>}>,
                RvvFloatInsts>(nullptr, "vmfge.vf", ActionTags::EXECUTE_TAG));

        // ternary operations

        auto maccWrapper = []<auto mulAdd>(auto src2, auto src1, auto dst)
        {
            return mulAdd(src2, src1, dst).v;
        };
        inst_handlers.emplace(
            "vfmacc.vv",
            pegasus::Action::createAction<
                &RvvFloatInsts::vfTernaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::V},
                    maccWrapper>,
                RvvFloatInsts>(nullptr, "vfmacc.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfmacc.vf",
            pegasus::Action::createAction<
                &RvvFloatInsts::vfTernaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::F},
                    maccWrapper>,
                RvvFloatInsts>(nullptr, "vfmacc.vf", ActionTags::EXECUTE_TAG));

        auto nmaccWrapper = []<auto mulAdd>(auto src2, auto src1, auto dst)
        {
            return mulAdd(fnegate(src2), src1, fnegate(dst)).v;
        };
        inst_handlers.emplace(
            "vfnmacc.vv",
            pegasus::Action::createAction<
                &RvvFloatInsts::vfTernaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::V},
                    nmaccWrapper>,
                RvvFloatInsts>(nullptr, "vfnmacc.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfnmacc.vf",
            pegasus::Action::createAction<
                &RvvFloatInsts::vfTernaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::F},
                    nmaccWrapper>,
                RvvFloatInsts>(nullptr, "vfnmacc.vf", ActionTags::EXECUTE_TAG));

        auto msacWrapper = []<auto mulAdd>(auto src2, auto src1, auto dst)
        {
            return mulAdd(src2, src1, fnegate(dst)).v;
        };
        inst_handlers.emplace(
            "vfmsac.vv",
            pegasus::Action::createAction<
                &RvvFloatInsts::vfTernaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::V},
                    msacWrapper>,
                RvvFloatInsts>(nullptr, "vfmsac.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfmsac.vf",
            pegasus::Action::createAction<
                &RvvFloatInsts::vfTernaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::F},
                    msacWrapper>,
                RvvFloatInsts>(nullptr, "vfmsac.vf", ActionTags::EXECUTE_TAG));

        auto nmsacWrapper = []<auto mulAdd>(auto src2, auto src1, auto dst)
        {
            return mulAdd(fnegate(src2), src1, dst).v;
        };
        inst_handlers.emplace(
            "vfnmsac.vv",
            pegasus::Action::createAction<
                &RvvFloatInsts::vfTernaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::V},
                    nmsacWrapper>,
                RvvFloatInsts>(nullptr, "vfnmsac.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfnmsac.vf",
            pegasus::Action::createAction<
                &RvvFloatInsts::vfTernaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::F},
                    nmsacWrapper>,
                RvvFloatInsts>(nullptr, "vfnmsac.vf", ActionTags::EXECUTE_TAG));

        auto maddWrapper = []<auto mulAdd>(auto src2, auto src1, auto dst)
        {
            return mulAdd(src1, dst, src2).v;
        };
        inst_handlers.emplace(
            "vfmadd.vv",
            pegasus::Action::createAction<
                &RvvFloatInsts::vfTernaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::V},
                    maddWrapper>,
                RvvFloatInsts>(nullptr, "vfmadd.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfmadd.vf",
            pegasus::Action::createAction<
                &RvvFloatInsts::vfTernaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::F},
                    maddWrapper>,
                RvvFloatInsts>(nullptr, "vfmadd.vf", ActionTags::EXECUTE_TAG));

        auto nmaddWrapper = []<auto mulAdd>(auto src2, auto src1, auto dst)
        {
            return mulAdd(fnegate(src1), dst, fnegate(src2)).v;
        };
        inst_handlers.emplace(
            "vfnmadd.vv",
            pegasus::Action::createAction<
                &RvvFloatInsts::vfTernaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::V},
                    nmaddWrapper>,
                RvvFloatInsts>(nullptr, "vfnmadd.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfnmadd.vf",
            pegasus::Action::createAction<
                &RvvFloatInsts::vfTernaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::F},
                    nmaddWrapper>,
                RvvFloatInsts>(nullptr, "vfnmadd.vf", ActionTags::EXECUTE_TAG));

        auto msubWrapper = []<auto mulAdd>(auto src2, auto src1, auto dst)
        {
            return mulAdd(src1, dst, fnegate(src2)).v;
        };
        inst_handlers.emplace(
            "vfmsub.vv",
            pegasus::Action::createAction<
                &RvvFloatInsts::vfTernaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::V},
                    msubWrapper>,
                RvvFloatInsts>(nullptr, "vfmsub.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfmsub.vf",
            pegasus::Action::createAction<
                &RvvFloatInsts::vfTernaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::F},
                    msubWrapper>,
                RvvFloatInsts>(nullptr, "vfmsub.vf", ActionTags::EXECUTE_TAG));

        auto nmsubWrapper = []<auto mulAdd>(auto src2, auto src1, auto dst)
        {
            return mulAdd(fnegate(src1), dst, src2).v;
        };
        inst_handlers.emplace(
            "vfnmsub.vv",
            pegasus::Action::createAction<
                &RvvFloatInsts::vfTernaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::V},
                    nmsubWrapper>,
                RvvFloatInsts>(nullptr, "vfnmsub.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfnmsub.vf",
            pegasus::Action::createAction<
                &RvvFloatInsts::vfTernaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::F},
                    nmsubWrapper>,
                RvvFloatInsts>(nullptr, "vfnmsub.vf", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vfwmacc.vv",
            pegasus::Action::createAction<
                &RvvFloatInsts::vfTernaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::W, OperandMode::Mode::V, OperandMode::Mode::V},
                    maccWrapper>,
                RvvFloatInsts>(nullptr, "vfwmacc.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfwmacc.vf",
            pegasus::Action::createAction<
                &RvvFloatInsts::vfTernaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::W, OperandMode::Mode::V, OperandMode::Mode::F},
                    maccWrapper>,
                RvvFloatInsts>(nullptr, "vfwmacc.vf", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vfwnmacc.vv",
            pegasus::Action::createAction<
                &RvvFloatInsts::vfTernaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::W, OperandMode::Mode::V, OperandMode::Mode::V},
                    nmaccWrapper>,
                RvvFloatInsts>(nullptr, "vfwnmacc.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfwnmacc.vf",
            pegasus::Action::createAction<
                &RvvFloatInsts::vfTernaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::W, OperandMode::Mode::V, OperandMode::Mode::F},
                    nmaccWrapper>,
                RvvFloatInsts>(nullptr, "vfwnmacc.vf", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vfwmsac.vv",
            pegasus::Action::createAction<
                &RvvFloatInsts::vfTernaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::W, OperandMode::Mode::V, OperandMode::Mode::V},
                    msacWrapper>,
                RvvFloatInsts>(nullptr, "vfwmsac.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfwmsac.vf",
            pegasus::Action::createAction<
                &RvvFloatInsts::vfTernaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::W, OperandMode::Mode::V, OperandMode::Mode::F},
                    msacWrapper>,
                RvvFloatInsts>(nullptr, "vfwmsac.vf", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vfwnmsac.vv",
            pegasus::Action::createAction<
                &RvvFloatInsts::vfTernaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::W, OperandMode::Mode::V, OperandMode::Mode::V},
                    nmsacWrapper>,
                RvvFloatInsts>(nullptr, "vfwnmsac.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfwnmsac.vf",
            pegasus::Action::createAction<
                &RvvFloatInsts::vfTernaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::W, OperandMode::Mode::V, OperandMode::Mode::F},
                    nmsacWrapper>,
                RvvFloatInsts>(nullptr, "vfwnmsac.vf", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vfmin.vv",
            pegasus::Action::createAction<
                &RvvFloatInsts::vfBinaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::V},
                    FloatFuncs{fmin<float16_t>, fmin<float32_t>, fmin<float64_t>}>,
                RvvFloatInsts>(nullptr, "vfmin.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfmin.vf",
            pegasus::Action::createAction<
                &RvvFloatInsts::vfBinaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::F},
                    FloatFuncs{fmin<float16_t>, fmin<float32_t>, fmin<float64_t>}>,
                RvvFloatInsts>(nullptr, "vfmin.vf", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfmax.vv",
            pegasus::Action::createAction<
                &RvvFloatInsts::vfBinaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::V},
                    FloatFuncs{fmax<float16_t>, fmax<float32_t>, fmax<float64_t>}>,
                RvvFloatInsts>(nullptr, "vfmax.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vfmax.vf",
            pegasus::Action::createAction<
                &RvvFloatInsts::vfBinaryHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::F},
                    FloatFuncs{fmax<float16_t>, fmax<float32_t>, fmax<float64_t>}>,
                RvvFloatInsts>(nullptr, "vfmax.vf", ActionTags::EXECUTE_TAG));
    }

    template void RvvFloatInsts::getInstHandlers<RV32>(std::map<std::string, Action> &);
    template void RvvFloatInsts::getInstHandlers<RV64>(std::map<std::string, Action> &);

    template <typename XLEN, size_t elemWidth>
    Action::ItrType vfmvHelper(pegasus::PegasusState* state, Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        Elements<Element<elemWidth>, false> elems_vd{state, state->getVectorConfig(),
                                                     inst->getRd()};

        for (auto iter = elems_vd.begin(); iter != elems_vd.end(); ++iter)
        {
            auto index = iter.getIndex();
            elems_vd.getElement(index).setVal(
                static_cast<UintType<elemWidth>>(READ_FP_REG<XLEN>(state, inst->getRs1())));
        }

        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvvFloatInsts::vfmvHandler_(pegasus::PegasusState* state,
                                                Action::ItrType action_it)
    {
        VectorConfig* vector_config = state->getVectorConfig();
        switch (vector_config->getSEW())
        {
            case 16:
                return vfmvHelper<XLEN, 16>(state, action_it);
            case 32:
                return vfmvHelper<XLEN, 32>(state, action_it);
            case 64:
                return vfmvHelper<XLEN, 64>(state, action_it);
            default:
                sparta_assert(false, "Unsupported SEW value");
                break;
        }
        return ++action_it;
    }

    template <typename XLEN, size_t elemWidth>
    Action::ItrType vfmergeHelper(pegasus::PegasusState* state, Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        Elements<Element<elemWidth>, false> elems_vs2{state, state->getVectorConfig(),
                                                      inst->getRs2()};
        Elements<Element<elemWidth>, false> elems_vd{state, state->getVectorConfig(),
                                                     inst->getRd()};
        const UintType<elemWidth> f =
            static_cast<UintType<elemWidth>>(READ_FP_REG<XLEN>(state, inst->getRs1()));
        const MaskElements mask_elems{state, state->getVectorConfig(), pegasus::V0};

        for (auto iter = elems_vd.begin(); iter != elems_vd.end(); ++iter)
        {
            auto index = iter.getIndex();
            if (mask_elems.getBit(index))
            {
                elems_vd.getElement(index).setVal(f);
            }
            else
            {
                elems_vd.getElement(index).setVal(elems_vd.getElement(index).getVal());
            }
        }

        return ++action_it;
    }

    template <typename XLEN>
    Action::ItrType RvvFloatInsts::vfmergeHandler_(pegasus::PegasusState* state,
                                                   Action::ItrType action_it)
    {
        VectorConfig* vector_config = state->getVectorConfig();
        switch (vector_config->getSEW())
        {
            case 16:
                return vfmergeHelper<XLEN, 16>(state, action_it);

            case 32:
                return vfmergeHelper<XLEN, 32>(state, action_it);

            case 64:
                return vfmergeHelper<XLEN, 64>(state, action_it);

            default:
                sparta_assert(false, "Unsupported SEW value");
                break;
        }
        return ++action_it;
    }

    template <typename XLEN, size_t elemWidth, OperandMode opMode, auto func>
    Action::ItrType vfUnaryHelper(pegasus::PegasusState* state, Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        Elements<Element<opMode.src2 == OperandMode::Mode::W ? 2 * elemWidth : elemWidth>, false>
            elems_vs2{state, state->getVectorConfig(), inst->getRs2()};
        Elements<Element<opMode.dst == OperandMode::Mode::W ? 2 * elemWidth : elemWidth>, false>
            elems_vd{state, state->getVectorConfig(), inst->getRd()};
        softfloat_roundingMode = READ_CSR_REG<XLEN>(state, FRM);

        auto execute = [&](auto iter, const auto & end)
        {
            for (; iter != end; ++iter)
            {
                auto index = iter.getIndex();
                elems_vd.getElement(index).setVal(func(elems_vs2.getElement(index).getVal()));
            }
        };

        if (inst->getVM()) // unmasked
        {
            execute(elems_vs2.begin(), elems_vs2.end());
        }
        else // masked
        {
            const MaskElements mask_elems{state, state->getVectorConfig(), pegasus::V0};
            execute(mask_elems.maskBitIterBegin(), mask_elems.maskBitIterEnd());
        }

        updateFloatCsrs<XLEN>(state);

        return ++action_it;
    }

    template <typename XLEN, OperandMode opMode, RvvFloatInsts::FloatFuncs funcs>
    Action::ItrType RvvFloatInsts::vfUnaryHandler_(pegasus::PegasusState* state,
                                                   Action::ItrType action_it)
    {
        VectorConfig* vector_config = state->getVectorConfig();

        switch (vector_config->getSEW())
        {
            case 16:
                if constexpr (funcs.f16 == nullptr)
                {
                    sparta_assert(false, "Unsupported SEW value");
                }
                else
                {
                    return vfUnaryHelper<XLEN, 16, opMode, [](auto src2) {
                        return func_wrapper(funcs.f16, src2);
                    }>(state, action_it);
                }
                break;
            case 32:
                return vfUnaryHelper<XLEN, 32, opMode, [](auto src2) {
                    return func_wrapper(funcs.f32, src2);
                }>(state, action_it);

            case 64:
                // neither narrowing to 64 bit nor widening from 64 bit
                if constexpr (opMode.dst != OperandMode::Mode::W
                              && opMode.src2 != OperandMode::Mode::W)
                {
                    return vfUnaryHelper<XLEN, 64, opMode, [](auto src2) {
                        return func_wrapper(funcs.f64, src2);
                    }>(state, action_it);
                }
            default:
                sparta_assert(false, "Unsupported SEW value");
                break;
        }
        return ++action_it;
    }

    template <typename XLEN, size_t elemWidth, OperandMode opMode, auto func>
    Action::ItrType vfFloatToIntHelper(pegasus::PegasusState* state, Action::ItrType action_it)
    {
        using Traits = FuncTraits<decltype(func)>;
        using ArgType = std::tuple_element_t<0, typename Traits::ArgsTuple>;
        using IntT = decltype(std::declval<ArgType>().v);

        const PegasusInstPtr & inst = state->getCurrentInst();
        Elements<Element<opMode.src2 == OperandMode::Mode::W ? 2 * elemWidth : elemWidth>, false>
            elems_vs2{state, state->getVectorConfig(), inst->getRs2()};
        Elements<Element<opMode.dst == OperandMode::Mode::W ? 2 * elemWidth : elemWidth>, false>
            elems_vd{state, state->getVectorConfig(), inst->getRd()};
        softfloat_roundingMode = READ_CSR_REG<XLEN>(state, FRM);

        auto execute = [&](auto iter, const auto & end)
        {
            for (; iter != end; ++iter)
            {
                auto index = iter.getIndex();
                elems_vd.getElement(index).setVal(
                    func(ArgType{static_cast<IntT>(elems_vs2.getElement(index).getVal())},
                         softfloat_roundingMode, true));
            }
        };

        if (inst->getVM()) // unmasked
        {
            execute(elems_vs2.begin(), elems_vs2.end());
        }
        else // masked
        {
            const MaskElements mask_elems{state, state->getVectorConfig(), pegasus::V0};
            execute(mask_elems.maskBitIterBegin(), mask_elems.maskBitIterEnd());
        }

        updateFloatCsrs<XLEN>(state);

        return ++action_it;
    }

    template <typename XLEN, OperandMode opMode, RvvFloatInsts::FloatFuncs funcs>
    Action::ItrType RvvFloatInsts::vfFloatToIntHandler_(pegasus::PegasusState* state,
                                                        Action::ItrType action_it)
    {
        VectorConfig* vector_config = state->getVectorConfig();

        switch (vector_config->getSEW())
        {
            case 16:
                if constexpr (funcs.f16 == nullptr)
                {
                    sparta_assert(false, "Unsupported SEW value");
                }
                else
                {
                    return vfFloatToIntHelper<XLEN, 16, opMode, funcs.f16>(state, action_it);
                }
                break;
            case 32:
                return vfFloatToIntHelper<XLEN, 32, opMode, funcs.f32>(state, action_it);

            case 64:
                // neither narrowing to 64 bit nor widening from 64 bit
                if constexpr (opMode.dst != OperandMode::Mode::W
                              && opMode.src2 != OperandMode::Mode::W)
                {
                    return vfFloatToIntHelper<XLEN, 64, opMode, funcs.f64>(state, action_it);
                }
            default:
                sparta_assert(false, "Unsupported SEW value");
                break;
        }
        return ++action_it;
    }

    template <typename XLEN, size_t elemWidth, OperandMode opMode, auto func>
    Action::ItrType vfBinaryHelper(pegasus::PegasusState* state, Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        Elements<Element<elemWidth>, false> elems_vs1{state, state->getVectorConfig(),
                                                      inst->getRs1()};
        Elements<Element<opMode.src2 == OperandMode::Mode::W ? 2 * elemWidth : elemWidth>, false>
            elems_vs2{state, state->getVectorConfig(), inst->getRs2()};
        Elements<Element<opMode.dst == OperandMode::Mode::W ? 2 * elemWidth : elemWidth>, false>
            elems_vd{state, state->getVectorConfig(), inst->getRd()};
        softfloat_roundingMode = READ_CSR_REG<XLEN>(state, FRM);

        auto execute = [&](auto iter, const auto & end)
        {
            for (; iter != end; ++iter)
            {
                auto index = iter.getIndex();
                typename decltype(elems_vd)::ElemType::ValueType value = 0;
                if constexpr (opMode.src1 == OperandMode::Mode::V)
                {
                    value = func(elems_vs2.getElement(index).getVal(),
                                 elems_vs1.getElement(index).getVal());
                }
                else if constexpr (opMode.src1 == OperandMode::Mode::F)
                {
                    value = func(
                        elems_vs2.getElement(index).getVal(),
                        static_cast<UintType<elemWidth>>(READ_FP_REG<RV64>(state, inst->getRs1())));
                }
                elems_vd.getElement(index).setVal(value);
            }
        };

        if (inst->getVM()) // unmasked
        {
            execute(elems_vs2.begin(), elems_vs2.end());
        }
        else // masked
        {
            const MaskElements mask_elems{state, state->getVectorConfig(), pegasus::V0};
            execute(mask_elems.maskBitIterBegin(), mask_elems.maskBitIterEnd());
        }

        updateFloatCsrs<XLEN>(state);

        return ++action_it;
    }

    template <typename XLEN, OperandMode opMode, RvvFloatInsts::FloatFuncs funcs>
    Action::ItrType RvvFloatInsts::vfBinaryHandler_(pegasus::PegasusState* state,
                                                    Action::ItrType action_it)
    {
        VectorConfig* vector_config = state->getVectorConfig();
        switch (vector_config->getSEW())
        {
            case 16:
                return vfBinaryHelper<XLEN, 16, opMode,
                                      [](auto src2, auto src1)
                                      {
                                          if constexpr (opMode.dst == OperandMode::Mode::W)
                                          {
                                              return funcs.f32(float32_t{src2}, float32_t{src1}).v;
                                          }
                                          else
                                          {
                                              return funcs.f16(float16_t{src2}, float16_t{src1}).v;
                                          }
                                      }>(state, action_it);

            case 32:
                return vfBinaryHelper<XLEN, 32, opMode,
                                      [](auto src2, auto src1)
                                      {
                                          if constexpr (opMode.dst == OperandMode::Mode::W)
                                          {
                                              return funcs.f64(float64_t{src2}, float64_t{src1}).v;
                                          }
                                          else
                                          {
                                              return funcs.f32(float32_t{src2}, float32_t{src1}).v;
                                          }
                                      }>(state, action_it);

            case 64:
                if constexpr (opMode.dst != OperandMode::Mode::W)
                {
                    return vfBinaryHelper<XLEN, 64, opMode, [](auto src2, auto src1) {
                        return funcs.f64(float64_t{src2}, float64_t{src1}).v;
                    }>(state, action_it);
                }
            default:
                sparta_assert(false, "Unsupported SEW value");
                break;
        }
        return ++action_it;
    }

    template <typename XLEN, RvvFloatInsts::FloatFuncs funcs>
    Action::ItrType RvvFloatInsts::vfrBinaryHandler_(pegasus::PegasusState* state,
                                                     Action::ItrType action_it)
    {
        VectorConfig* vector_config = state->getVectorConfig();
        switch (vector_config->getSEW())
        {
            case 16:
                return vfBinaryHelper<XLEN, 16,
                                      OperandMode{OperandMode::Mode::V, OperandMode::Mode::V,
                                                  OperandMode::Mode::F},
                                      [](auto src2, auto src1) {
                                          return funcs.f16(float16_t{src1}, float16_t{src2}).v;
                                      }>(state, action_it);

            case 32:
                return vfBinaryHelper<XLEN, 32,
                                      OperandMode{OperandMode::Mode::V, OperandMode::Mode::V,
                                                  OperandMode::Mode::F},
                                      [](auto src2, auto src1) {
                                          return funcs.f32(float32_t{src1}, float32_t{src2}).v;
                                      }>(state, action_it);

            case 64:
                return vfBinaryHelper<XLEN, 64,
                                      OperandMode{OperandMode::Mode::V, OperandMode::Mode::V,
                                                  OperandMode::Mode::F},
                                      [](auto src2, auto src1) {
                                          return funcs.f64(float64_t{src1}, float64_t{src2}).v;
                                      }>(state, action_it);

            default:
                sparta_assert(false, "Unsupported SEW value");
                break;
        }
        return ++action_it;
    }

    template <typename XLEN, size_t elemWidth, OperandMode opMode, auto func>
    Action::ItrType vmfbinaryHelper(PegasusState* state, Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        Elements<Element<elemWidth>, false> elems_vs1{state, state->getVectorConfig(),
                                                      inst->getRs1()};
        Elements<Element<elemWidth>, false> elems_vs2{state, state->getVectorConfig(),
                                                      inst->getRs2()};
        MaskElements elems_vd{state, state->getVectorConfig(), inst->getRd()};
        softfloat_roundingMode = READ_CSR_REG<XLEN>(state, FRM);

        auto execute = [&](auto iter, const auto & end)
        {
            for (; iter != end; ++iter)
            {
                auto index = iter.getIndex();
                if constexpr (opMode.src1 == OperandMode::Mode::V)
                {
                    elems_vd.getElement(index).setBit(func(elems_vs2.getElement(index).getVal(),
                                                           elems_vs1.getElement(index).getVal()));
                }
                else if constexpr (opMode.src1 == OperandMode::Mode::F)
                {
                    elems_vd.getElement(index).setBit(
                        func(elems_vs2.getElement(index).getVal(),
                             static_cast<UintType<elemWidth>>(
                                 READ_FP_REG<RV64>(state, inst->getRs1()))));
                }
            }
        };

        if (inst->getVM()) // unmasked
        {
            execute(elems_vs2.begin(), elems_vs2.end());
        }
        else // masked
        {
            const MaskElements mask_elems{state, state->getVectorConfig(), pegasus::V0};
            execute(mask_elems.maskBitIterBegin(), mask_elems.maskBitIterEnd());
        }

        updateFloatCsrs<XLEN>(state);

        return ++action_it;
    }

    template <typename XLEN, OperandMode opMode, RvvFloatInsts::FloatFuncs funcs>
    Action::ItrType RvvFloatInsts::vmfBinaryHandler_(pegasus::PegasusState* state,
                                                     Action::ItrType action_it)
    {
        VectorConfig* vector_config = state->getVectorConfig();
        switch (vector_config->getSEW())
        {
            case 16:
                return vfBinaryHelper<XLEN, 16, opMode, [](auto src2, auto src1) {
                    return funcs.f16(float16_t{src2}, float16_t{src1});
                }>(state, action_it);

            case 32:
                return vfBinaryHelper<XLEN, 32, opMode, [](auto src2, auto src1) {
                    return funcs.f32(float32_t{src2}, float32_t{src1});
                }>(state, action_it);

            case 64:
                return vfBinaryHelper<XLEN, 64, opMode, [](auto src2, auto src1) {
                    return funcs.f64(float64_t{src2}, float64_t{src1});
                }>(state, action_it);

            default:
                sparta_assert(false, "Unsupported SEW value");
                break;
        }
        return ++action_it;
    }

    template <typename XLEN, size_t elemWidth, OperandMode opMode, auto func>
    Action::ItrType vfTernaryHelper(pegasus::PegasusState* state, Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        Elements<Element<elemWidth>, false> elems_vs1{state, state->getVectorConfig(),
                                                      inst->getRs1()};
        Elements<Element<elemWidth>, false> elems_vs2{state, state->getVectorConfig(),
                                                      inst->getRs2()};
        Elements<Element<opMode.dst == OperandMode::Mode::W ? 2 * elemWidth : elemWidth>, false>
            elems_vd{state, state->getVectorConfig(), inst->getRd()};
        softfloat_roundingMode = READ_CSR_REG<XLEN>(state, FRM);

        auto execute = [&](auto iter, const auto & end)
        {
            for (; iter != end; ++iter)
            {
                auto index = iter.getIndex();
                auto elem_vd = elems_vd.getElement(index);
                typename Element<elemWidth>::ValueType value = 0;
                if constexpr (opMode.src1 == OperandMode::Mode::V)
                {
                    value = func(elems_vs2.getElement(index).getVal(),
                                 elems_vs1.getElement(index).getVal(), elem_vd.getVal());
                }
                else if constexpr (opMode.src1 == OperandMode::Mode::F)
                {
                    value = func(
                        elems_vs2.getElement(index).getVal(),
                        static_cast<UintType<elemWidth>>(READ_FP_REG<RV64>(state, inst->getRs1())),
                        elem_vd.getVal());
                }
                elem_vd.setVal(value);
            }
        };

        if (inst->getVM()) // unmasked
        {
            execute(elems_vs2.begin(), elems_vs2.end());
        }
        else // masked
        {
            const MaskElements mask_elems{state, state->getVectorConfig(), pegasus::V0};
            execute(mask_elems.maskBitIterBegin(), mask_elems.maskBitIterEnd());
        }

        updateFloatCsrs<XLEN>(state);

        return ++action_it;
    }

    template <typename XLEN, OperandMode opMode, auto funcWrapper>
    Action::ItrType RvvFloatInsts::vfTernaryHandler_(pegasus::PegasusState* state,
                                                     Action::ItrType action_it)
    {
        VectorConfig* vector_config = state->getVectorConfig();

        switch (vector_config->getSEW())
        {
            case 16:
                return vfTernaryHelper<XLEN, 16, opMode,
                                       [](auto src2, auto src1, auto dst)
                                       {
                                           if constexpr (opMode.dst == OperandMode::Mode::W)
                                           {
                                               return funcWrapper.template operator()<f32_mulAdd>(
                                                   float32_t{src2}, float32_t{src1},
                                                   float32_t{dst});
                                           }
                                           else
                                           {
                                               return funcWrapper.template operator()<f16_mulAdd>(
                                                   float16_t{src2}, float16_t{src1},
                                                   float16_t{dst});
                                           }
                                       }>(state, action_it);

            case 32:
                return vfTernaryHelper<XLEN, 32, opMode,
                                       [](auto src2, auto src1, auto dst)
                                       {
                                           if constexpr (opMode.dst == OperandMode::Mode::W)
                                           {
                                               return funcWrapper.template operator()<f64_mulAdd>(
                                                   float64_t{src2}, float64_t{src1},
                                                   float64_t{dst});
                                           }
                                           else
                                           {
                                               return funcWrapper.template operator()<f32_mulAdd>(
                                                   float32_t{src2}, float32_t{src1},
                                                   float32_t{dst});
                                           }
                                       }>(state, action_it);

            case 64:
                if constexpr (opMode.dst != OperandMode::Mode::W)
                {
                    return vfTernaryHelper<XLEN, 64, opMode,
                                           [](auto src2, auto src1, auto dst)
                                           {
                                               return funcWrapper.template operator()<f64_mulAdd>(
                                                   float64_t{src2}, float64_t{src1},
                                                   float64_t{dst});
                                           }>(state, action_it);
                }
            default:
                sparta_assert(false, "Unsupported SEW value");
                break;
        }

        return ++action_it;
    }
} // namespace pegasus
