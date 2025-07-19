#include <limits>
#include <functional>

#include "core/inst_handlers/v/RvvIntegerInsts.hpp"
#include "core/PegasusState.hpp"
#include "core/ActionGroup.hpp"
#include "core/VecElements.hpp"
#include "include/ActionTags.hpp"

namespace pegasus
{
    template <typename XLEN>
    void RvvIntegerInsts::getInstHandlers(std::map<std::string, Action> & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);

        inst_handlers.emplace(
            "vadd.vv",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viablHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::V},
                    false, std::plus>,
                RvvIntegerInsts>(nullptr, "vadd.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vadd.vx",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viablHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::X},
                    false, std::plus>,
                RvvIntegerInsts>(nullptr, "vadd.vx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vadd.vi",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viablHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::I},
                    false, std::plus>,
                RvvIntegerInsts>(nullptr, "vadd.vi", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vand.vv",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viablHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::V},
                    false, std::bit_and>,
                RvvIntegerInsts>(nullptr, "vand.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vand.vx",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viablHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::X},
                    false, std::bit_and>,
                RvvIntegerInsts>(nullptr, "vand.vx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vand.vi",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viablHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::I},
                    false, std::bit_and>,
                RvvIntegerInsts>(nullptr, "vand.vi", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vor.vv",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viablHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::V},
                    false, std::bit_or>,
                RvvIntegerInsts>(nullptr, "vor.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vor.vx",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viablHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::X},
                    false, std::bit_or>,
                RvvIntegerInsts>(nullptr, "vor.vx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vor.vi",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viablHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::I},
                    false, std::bit_or>,
                RvvIntegerInsts>(nullptr, "vor.vi", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vxor.vv",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viablHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::V},
                    false, std::bit_xor>,
                RvvIntegerInsts>(nullptr, "vxor.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vxor.vx",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viablHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::X},
                    false, std::bit_xor>,
                RvvIntegerInsts>(nullptr, "vxor.vx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vxor.vi",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viablHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::I},
                    false, std::bit_xor>,
                RvvIntegerInsts>(nullptr, "vxor.vi", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vsub.vv",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viablHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::V},
                    false, std::minus>,
                RvvIntegerInsts>(nullptr, "vsub.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vsub.vx",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viablHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::X},
                    false, std::minus>,
                RvvIntegerInsts>(nullptr, "vsub.vx", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vwadd.vv",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viablHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::W, OperandMode::Mode::V, OperandMode::Mode::V},
                    true, std::plus>,
                RvvIntegerInsts>(nullptr, "vwadd.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vwadd.vx",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viablHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::W, OperandMode::Mode::V, OperandMode::Mode::X},
                    true, std::plus>,
                RvvIntegerInsts>(nullptr, "vwadd.vx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vwadd.wv",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viablHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::W, OperandMode::Mode::W, OperandMode::Mode::V},
                    true, std::plus>,
                RvvIntegerInsts>(nullptr, "vwadd.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vwadd.wx",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viablHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::W, OperandMode::Mode::W, OperandMode::Mode::X},
                    true, std::plus>,
                RvvIntegerInsts>(nullptr, "vwadd.vx", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vwaddu.vv",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viablHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::W, OperandMode::Mode::V, OperandMode::Mode::V},
                    false, std::plus>,
                RvvIntegerInsts>(nullptr, "vwaddu.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vwaddu.vx",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viablHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::W, OperandMode::Mode::V, OperandMode::Mode::X},
                    false, std::plus>,
                RvvIntegerInsts>(nullptr, "vwaddu.vx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vwaddu.wv",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viablHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::W, OperandMode::Mode::W, OperandMode::Mode::V},
                    false, std::plus>,
                RvvIntegerInsts>(nullptr, "vwaddu.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vwaddu.wx",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viablHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::W, OperandMode::Mode::W, OperandMode::Mode::X},
                    false, std::plus>,
                RvvIntegerInsts>(nullptr, "vwaddu.vx", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vwsub.vv",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viablHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::W, OperandMode::Mode::V, OperandMode::Mode::V},
                    true, std::plus>,
                RvvIntegerInsts>(nullptr, "vwsub.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vwsub.vx",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viablHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::W, OperandMode::Mode::V, OperandMode::Mode::X},
                    true, std::plus>,
                RvvIntegerInsts>(nullptr, "vwsub.vx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vwsub.wv",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viablHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::W, OperandMode::Mode::W, OperandMode::Mode::V},
                    true, std::plus>,
                RvvIntegerInsts>(nullptr, "vwsub.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vwsub.wx",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viablHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::W, OperandMode::Mode::W, OperandMode::Mode::X},
                    true, std::plus>,
                RvvIntegerInsts>(nullptr, "vwsub.vx", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vwsubu.vv",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viablHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::W, OperandMode::Mode::V, OperandMode::Mode::V},
                    false, std::plus>,
                RvvIntegerInsts>(nullptr, "vwsubu.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vwsubu.vx",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viablHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::W, OperandMode::Mode::V, OperandMode::Mode::X},
                    false, std::plus>,
                RvvIntegerInsts>(nullptr, "vwsubu.vx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vwsubu.wv",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viablHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::W, OperandMode::Mode::W, OperandMode::Mode::V},
                    false, std::plus>,
                RvvIntegerInsts>(nullptr, "vwsubu.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vwsubu.wx",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viablHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::W, OperandMode::Mode::W, OperandMode::Mode::X},
                    false, std::plus>,
                RvvIntegerInsts>(nullptr, "vwsubu.vx", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vadc.vvm",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viacsbHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::V},
                    true, std::plus>,
                RvvIntegerInsts>(nullptr, "vadc.vvm", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vadc.vxm",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viacsbHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::X},
                    true, std::plus>,
                RvvIntegerInsts>(nullptr, "vadc.vxm", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vadc.vim",
            pegasus::Action::createAction<
                &RvvIntegerInsts::viacsbHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::I},
                    true, std::plus>,
                RvvIntegerInsts>(nullptr, "vadc.vim", ActionTags::EXECUTE_TAG));

        auto carryFn = []<typename T>(T a, T b, T c)
        { return a >= std::numeric_limits<T>::max() - b - c; };

        inst_handlers.emplace(
            "vmadc.vvm",
            pegasus::Action::createAction<
                &RvvIntegerInsts::vmiacsbHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::V},
                    true, carryFn>,
                RvvIntegerInsts>(nullptr, "vmadc.vvm", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmadc.vxm",
            pegasus::Action::createAction<
                &RvvIntegerInsts::vmiacsbHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::X},
                    true, carryFn>,
                RvvIntegerInsts>(nullptr, "vmadc.vxm", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmadc.vim",
            pegasus::Action::createAction<
                &RvvIntegerInsts::vmiacsbHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::I},
                    true, carryFn>,
                RvvIntegerInsts>(nullptr, "vmadc.vim", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmadc.vv",
            pegasus::Action::createAction<
                &RvvIntegerInsts::vmiacsbHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::V},
                    false, carryFn>,
                RvvIntegerInsts>(nullptr, "vmadc.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmadc.vx",
            pegasus::Action::createAction<
                &RvvIntegerInsts::vmiacsbHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::X},
                    false, carryFn>,
                RvvIntegerInsts>(nullptr, "vmadc.vx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmadc.vi",
            pegasus::Action::createAction<
                &RvvIntegerInsts::vmiacsbHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::I},
                    false, carryFn>,
                RvvIntegerInsts>(nullptr, "vmadc.vi", ActionTags::EXECUTE_TAG));

        auto borrowFn = []<typename T>(T a, T b, T c)
        { return (b + c == std::numeric_limits<T>::max()) || (a < b + c); };

        inst_handlers.emplace(
            "vmsbc.vvm",
            pegasus::Action::createAction<
                &RvvIntegerInsts::vmiacsbHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::V},
                    true, borrowFn>,
                RvvIntegerInsts>(nullptr, "vmsbc.vvm", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmsbc.vxm",
            pegasus::Action::createAction<
                &RvvIntegerInsts::vmiacsbHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::X},
                    true, borrowFn>,
                RvvIntegerInsts>(nullptr, "vmsbc.vxm", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmsbc.vv",
            pegasus::Action::createAction<
                &RvvIntegerInsts::vmiacsbHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::V},
                    false, borrowFn>,
                RvvIntegerInsts>(nullptr, "vmsbc.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmsbc.vx",
            pegasus::Action::createAction<
                &RvvIntegerInsts::vmiacsbHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::X},
                    false, borrowFn>,
                RvvIntegerInsts>(nullptr, "vmsbc.vx", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vmseq.vv",
            pegasus::Action::createAction<
                &RvvIntegerInsts::vmicHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::V},
                    true, std::equal_to>,
                RvvIntegerInsts>(nullptr, "vmseq.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmseq.vx",
            pegasus::Action::createAction<
                &RvvIntegerInsts::vmicHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::X},
                    true, std::equal_to>,
                RvvIntegerInsts>(nullptr, "vmseq.vx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmseq.vi",
            pegasus::Action::createAction<
                &RvvIntegerInsts::vmicHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::I},
                    true, std::equal_to>,
                RvvIntegerInsts>(nullptr, "vmseq.vi", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vmsne.vv",
            pegasus::Action::createAction<
                &RvvIntegerInsts::vmicHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::V},
                    true, std::not_equal_to>,
                RvvIntegerInsts>(nullptr, "vmsne.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmsne.vx",
            pegasus::Action::createAction<
                &RvvIntegerInsts::vmicHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::X},
                    true, std::not_equal_to>,
                RvvIntegerInsts>(nullptr, "vmsne.vx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmsne.vi",
            pegasus::Action::createAction<
                &RvvIntegerInsts::vmicHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::I},
                    true, std::not_equal_to>,
                RvvIntegerInsts>(nullptr, "vmsne.vi", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vmsltu.vv",
            pegasus::Action::createAction<
                &RvvIntegerInsts::vmicHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::V},
                    false, std::less>,
                RvvIntegerInsts>(nullptr, "vmsltu.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmsltu.vx",
            pegasus::Action::createAction<
                &RvvIntegerInsts::vmicHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::X},
                    false, std::less>,
                RvvIntegerInsts>(nullptr, "vmsltu.vx", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vmslt.vv",
            pegasus::Action::createAction<
                &RvvIntegerInsts::vmicHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::V},
                    true, std::less>,
                RvvIntegerInsts>(nullptr, "vmslt.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmslt.vx",
            pegasus::Action::createAction<
                &RvvIntegerInsts::vmicHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::X},
                    true, std::less>,
                RvvIntegerInsts>(nullptr, "vmslt.vx", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vmsleu.vv",
            pegasus::Action::createAction<
                &RvvIntegerInsts::vmicHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::V},
                    false, std::less_equal>,
                RvvIntegerInsts>(nullptr, "vmsleu.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmsleu.vx",
            pegasus::Action::createAction<
                &RvvIntegerInsts::vmicHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::X},
                    false, std::less_equal>,
                RvvIntegerInsts>(nullptr, "vmsleu.vx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmsleu.vi",
            pegasus::Action::createAction<
                &RvvIntegerInsts::vmicHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::I},
                    false, std::less_equal>,
                RvvIntegerInsts>(nullptr, "vmsleu.vi", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vmsle.vv",
            pegasus::Action::createAction<
                &RvvIntegerInsts::vmicHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::V},
                    true, std::less_equal>,
                RvvIntegerInsts>(nullptr, "vmsle.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmsle.vx",
            pegasus::Action::createAction<
                &RvvIntegerInsts::vmicHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::X},
                    true, std::less_equal>,
                RvvIntegerInsts>(nullptr, "vmsle.vx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmsle.vi",
            pegasus::Action::createAction<
                &RvvIntegerInsts::vmicHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::I},
                    true, std::less_equal>,
                RvvIntegerInsts>(nullptr, "vmsle.vi", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vmsgtu.vv",
            pegasus::Action::createAction<
                &RvvIntegerInsts::vmicHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::V},
                    false, std::greater>,
                RvvIntegerInsts>(nullptr, "vmsgtu.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmsgtu.vx",
            pegasus::Action::createAction<
                &RvvIntegerInsts::vmicHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::X},
                    false, std::greater>,
                RvvIntegerInsts>(nullptr, "vmsgtu.vx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmsgtu.vi",
            pegasus::Action::createAction<
                &RvvIntegerInsts::vmicHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::I},
                    false, std::greater>,
                RvvIntegerInsts>(nullptr, "vmsgtu.vi", ActionTags::EXECUTE_TAG));

        inst_handlers.emplace(
            "vmsgt.vv",
            pegasus::Action::createAction<
                &RvvIntegerInsts::vmicHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::V},
                    true, std::greater>,
                RvvIntegerInsts>(nullptr, "vmsgt.vv", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmsgt.vx",
            pegasus::Action::createAction<
                &RvvIntegerInsts::vmicHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::X},
                    true, std::greater>,
                RvvIntegerInsts>(nullptr, "vmsgt.vx", ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "vmsgt.vi",
            pegasus::Action::createAction<
                &RvvIntegerInsts::vmicHandler_<
                    XLEN,
                    OperandMode{OperandMode::Mode::V, OperandMode::Mode::V, OperandMode::Mode::I},
                    true, std::greater>,
                RvvIntegerInsts>(nullptr, "vmsgt.vi", ActionTags::EXECUTE_TAG));
    }

    template void RvvIntegerInsts::getInstHandlers<RV32>(std::map<std::string, Action> &);
    template void RvvIntegerInsts::getInstHandlers<RV64>(std::map<std::string, Action> &);

    template <typename XLEN, size_t elemWidth, OperandMode opMode, typename Functor>
    Action::ItrType viablHelper(PegasusState* state, Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        Elements<Element<elemWidth>, false> elems_vs1{state, state->getVectorConfig(),
                                                      inst->getRs1()};
        Elements<Element<opMode.src2 == OperandMode::Mode::W ? 2 * elemWidth : elemWidth>, false>
            elems_vs2{state, state->getVectorConfig(), inst->getRs2()};
        Elements<Element<opMode.src2 == OperandMode::Mode::W ? 2 * elemWidth : elemWidth>, false>
            elems_vd{state, state->getVectorConfig(), inst->getRd()};

        auto execute = [&](auto iter, const auto & end)
        {
            size_t index = 0;
            for (; iter != end; ++iter)
            {
                index = iter.getIndex();
                if constexpr (opMode.src1 == OperandMode::Mode::V)
                {
                    elems_vd.getElement(index).setVal(
                        Functor()(elems_vs2.getElement(index).getVal(),
                                  elems_vs1.getElement(index).getVal()));
                }
                else if constexpr (opMode.src1 == OperandMode::Mode::X)
                {
                    elems_vd.getElement(index).setVal(
                        Functor()(elems_vs2.getElement(index).getVal(),
                                  static_cast<typename Functor::result_type>(
                                      READ_INT_REG<XLEN>(state, inst->getRs1()))));
                }
                else // opMode.src1 == OperandMode::Mode::I
                {
                    elems_vd.getElement(index).setVal(Functor()(
                        elems_vs2.getElement(index).getVal(),
                        static_cast<typename Functor::result_type>(inst->getImmediate())));
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

        return ++action_it;
    }

    template <typename XLEN, OperandMode opMode, bool isSigned,
              template <typename> typename FunctorTemp>
    Action::ItrType RvvIntegerInsts::viablHandler_(PegasusState* state, Action::ItrType action_it)
    {
        VectorConfig* vector_config = state->getVectorConfig();
        if constexpr (isSigned)
        {
            switch (vector_config->getSEW())
            {
                case 8:
                    return viablHelper<XLEN, 8, opMode,
                                       FunctorTemp<typename std::conditional_t<
                                           opMode.dst == OperandMode::Mode::W, int16_t, int8_t>>>(
                        state, action_it);

                case 16:
                    return viablHelper<XLEN, 16, opMode,
                                       FunctorTemp<typename std::conditional_t<
                                           opMode.dst == OperandMode::Mode::W, uint32_t, int16_t>>>(
                        state, action_it);

                case 32:
                    return viablHelper<XLEN, 32, opMode,
                                       FunctorTemp<typename std::conditional_t<
                                           opMode.dst == OperandMode::Mode::W, int64_t, int32_t>>>(
                        state, action_it);

                case 64:
                    if constexpr (opMode.dst == OperandMode::Mode::W)
                    {
                        sparta_assert(false, "Unsupported SEW value");
                    }
                    else
                    {
                        return viablHelper<XLEN, 64, opMode, FunctorTemp<int64_t>>(state,
                                                                                   action_it);
                    }
                    break;
                default:
                    sparta_assert(false, "Unsupported SEW value");
                    break;
            }
        }
        else // Unsigned
        {
            switch (vector_config->getSEW())
            {
                case 8:
                    return viablHelper<XLEN, 8, opMode,
                                       FunctorTemp<typename std::conditional_t<
                                           opMode.dst == OperandMode::Mode::W, uint16_t, uint8_t>>>(
                        state, action_it);

                case 16:
                    return viablHelper<
                        XLEN, 16, opMode,
                        FunctorTemp<typename std::conditional_t<opMode.dst == OperandMode::Mode::W,
                                                                uint32_t, uint16_t>>>(state,
                                                                                      action_it);

                case 32:
                    return viablHelper<
                        XLEN, 32, opMode,
                        FunctorTemp<typename std::conditional_t<opMode.dst == OperandMode::Mode::W,
                                                                uint64_t, uint32_t>>>(state,
                                                                                      action_it);

                case 64:
                    if constexpr (opMode.dst == OperandMode::Mode::W)
                    {
                        sparta_assert(false, "Unsupported SEW value");
                    }
                    else
                    {
                        return viablHelper<XLEN, 64, opMode, FunctorTemp<uint64_t>>(state,
                                                                                    action_it);
                    }
                    break;
                default:
                    sparta_assert(false, "Unsupported SEW value");
                    break;
            }
        }
        return ++action_it;
    }

    template <typename XLEN, size_t elemWidth, OperandMode opMode, bool hasMaskOp, typename Functor>
    Action::ItrType viacsbHelper(PegasusState* state, Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        MaskElements elems_v0{state, state->getVectorConfig(), pegasus::V0};
        Elements<Element<elemWidth>, false> elems_vs1{state, state->getVectorConfig(),
                                                      inst->getRs1()};
        Elements<Element<elemWidth>, false> elems_vs2{state, state->getVectorConfig(),
                                                      inst->getRs2()};
        Elements<Element<elemWidth>, false> elems_vd{state, state->getVectorConfig(),
                                                     inst->getRd()};

        for (auto iter = elems_vs2.begin(); iter != elems_vs2.end(); ++iter)
        {
            auto index = iter.getIndex();
            typename Element<elemWidth>::ValueType value = 0;
            if constexpr (opMode.src1 == OperandMode::Mode::V)
            {
                value = Functor()(elems_vs2.getElement(index).getVal(),
                                  elems_vs1.getElement(index).getVal());
            }
            else if constexpr (opMode.src1 == OperandMode::Mode::X)
            {
                value = Functor()(elems_vs2.getElement(index).getVal(),
                                  static_cast<typename Functor::result_type>(
                                      READ_INT_REG<XLEN>(state, inst->getRs1())));
            }
            else // opMode.src1 == OperandMode::Mode::I
            {
                value = Functor()(elems_vs2.getElement(index).getVal(),
                                  static_cast<typename Functor::result_type>(inst->getImmediate()));
            }
            if constexpr (hasMaskOp)
            {
                value = Functor()(
                    value, static_cast<typename Functor::result_type>(elems_v0.getBit(index)));
            }
            elems_vd.getElement(index).setVal(value);
        }

        return ++action_it;
    }

    template <typename XLEN, OperandMode opMode, bool hasMaskOp,
              template <typename> typename FunctorTemp>
    Action::ItrType RvvIntegerInsts::viacsbHandler_(PegasusState* state, Action::ItrType action_it)
    {
        VectorConfig* vector_config = state->getVectorConfig();
        switch (vector_config->getSEW())
        {
            case 8:
                return viacsbHelper<XLEN, 8, opMode, hasMaskOp, FunctorTemp<uint8_t>>(state,
                                                                                      action_it);

            case 16:
                return viacsbHelper<XLEN, 16, opMode, hasMaskOp, FunctorTemp<uint16_t>>(state,
                                                                                        action_it);

            case 32:
                return viacsbHelper<XLEN, 32, opMode, hasMaskOp, FunctorTemp<uint32_t>>(state,
                                                                                        action_it);

            case 64:
                return viacsbHelper<XLEN, 64, opMode, hasMaskOp, FunctorTemp<uint64_t>>(state,
                                                                                        action_it);

            default:
                sparta_assert(false, "Unsupported SEW value");
                break;
        }
        return ++action_it;
    }

    template <typename XLEN, size_t elemWidth, OperandMode opMode, bool hasMaskOp, auto detectFunc>
    Action::ItrType vmiacsbHelper(PegasusState* state, Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        MaskElements elems_v0{state, state->getVectorConfig(), pegasus::V0};
        Elements<Element<elemWidth>, false> elems_vs1{state, state->getVectorConfig(),
                                                      inst->getRs1()};
        Elements<Element<elemWidth>, false> elems_vs2{state, state->getVectorConfig(),
                                                      inst->getRs2()};
        MaskElements elems_vd{state, state->getVectorConfig(), inst->getRd()};

        for (auto iter = elems_vs2.begin(); iter != elems_vs2.end(); ++iter)
        {
            auto index = iter.getIndex();
            auto a = elems_vs2.getElement(index).getVal();
            decltype(a) b = 0, c = 0;
            if constexpr (opMode.src1 == OperandMode::Mode::V)
            {
                b = elems_vs1.getElement(index).getVal();
            }
            else if constexpr (opMode.src1 == OperandMode::Mode::X)
            {
                b = static_cast<decltype(b)>(READ_INT_REG<XLEN>(state, inst->getRs1()));
            }
            else // opMode.src1 == OperandMode::Mode::I
            {
                b = static_cast<decltype(b)>(inst->getImmediate());
            }
            if constexpr (hasMaskOp)
            {
                c = static_cast<decltype(b)>(elems_v0.getBit(index));
            }
            elems_vd.getElement(index).setVal(detectFunc(a, b, c));
        }

        return ++action_it;
    }

    template <typename XLEN, OperandMode opMode, bool hasMaskOp, auto detectFunc>
    Action::ItrType RvvIntegerInsts::vmiacsbHandler_(PegasusState* state, Action::ItrType action_it)
    {
        VectorConfig* vector_config = state->getVectorConfig();
        switch (vector_config->getSEW())
        {
            case 8:
                return vmiacsbHelper<XLEN, 8, opMode, hasMaskOp, detectFunc>(state, action_it);

            case 16:
                return vmiacsbHelper<XLEN, 16, opMode, hasMaskOp, detectFunc>(state, action_it);

            case 32:
                return vmiacsbHelper<XLEN, 32, opMode, hasMaskOp, detectFunc>(state, action_it);

            case 64:
                return vmiacsbHelper<XLEN, 64, opMode, hasMaskOp, detectFunc>(state, action_it);

            default:
                sparta_assert(false, "Unsupported SEW value");
                break;
        }
        return ++action_it;
    }

    template <typename XLEN, size_t elemWidth, OperandMode opMode, typename Functor>
    Action::ItrType vmicHelper(PegasusState* state, Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        Elements<Element<elemWidth>, false> elems_vs1{state, state->getVectorConfig(),
                                                      inst->getRs1()};
        Elements<Element<elemWidth>, false> elems_vs2{state, state->getVectorConfig(),
                                                      inst->getRs2()};
        MaskElements elems_vd{state, state->getVectorConfig(), inst->getRd()};

        auto execute = [&]<typename Iterator>(const Iterator & begin, const Iterator & end)
        {
            size_t index = 0;
            for (auto iter = begin; iter != end; ++iter)
            {
                index = iter.getIndex();
                if constexpr (opMode.src1 == OperandMode::Mode::V)
                {
                    elems_vd.getElement(index).setBit(
                        Functor()(elems_vs2.getElement(index).getVal(),
                                  elems_vs1.getElement(index).getVal()));
                }
                else if (opMode.src1 == OperandMode::Mode::X)
                {
                    elems_vd.getElement(index).setBit(
                        Functor()(elems_vs2.getElement(index).getVal(),
                                  READ_INT_REG<XLEN>(state, inst->getRs1())));
                }
                else // opMode.src1 == OperandMode::Mode::I
                {
                    elems_vd.getElement(index).setBit(
                        Functor()(elems_vs2.getElement(index).getVal(), inst->getImmediate()));
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

        return ++action_it;
    }

    template <typename XLEN, OperandMode opMode, bool isSigned,
              template <typename> typename FunctorTemp>
    Action::ItrType RvvIntegerInsts::vmicHandler_(PegasusState* state, Action::ItrType action_it)
    {
        VectorConfig* vector_config = state->getVectorConfig();
        if constexpr (isSigned)
        {
            switch (vector_config->getSEW())
            {
                case 8:
                    return vmicHelper<XLEN, 8, opMode, FunctorTemp<int8_t>>(state, action_it);

                case 16:
                    return vmicHelper<XLEN, 16, opMode, FunctorTemp<int16_t>>(state, action_it);

                case 32:
                    return vmicHelper<XLEN, 32, opMode, FunctorTemp<int32_t>>(state, action_it);

                case 64:
                    return vmicHelper<XLEN, 64, opMode, FunctorTemp<int64_t>>(state, action_it);

                default:
                    sparta_assert(false, "Unsupported SEW value");
                    break;
            }
        }
        else // Unsigned
        {
            switch (vector_config->getSEW())
            {
                case 8:
                    return vmicHelper<XLEN, 8, opMode, FunctorTemp<uint8_t>>(state, action_it);

                case 16:
                    return vmicHelper<XLEN, 16, opMode, FunctorTemp<uint16_t>>(state, action_it);

                case 32:
                    return vmicHelper<XLEN, 32, opMode, FunctorTemp<uint32_t>>(state, action_it);

                case 64:
                    return vmicHelper<XLEN, 64, opMode, FunctorTemp<uint64_t>>(state, action_it);

                default:
                    sparta_assert(false, "Unsupported SEW value");
                    break;
            }
        }
        return ++action_it;
    }

} // namespace pegasus
