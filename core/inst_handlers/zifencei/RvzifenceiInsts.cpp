#include "core/inst_handlers/zifencei/RvzifenceiInsts.hpp"
#include "include/ActionTags.hpp"
#include "core/ActionGroup.hpp"
#include "core/PegasusState.hpp"
#include "core/PegasusInst.hpp"

namespace pegasus
{
    template <typename XLEN>
    void RvzifenceiInsts::getInstHandlers(std::map<std::string, Action> & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);
        if constexpr (std::is_same_v<XLEN, RV64>)
        {
            inst_handlers.emplace(
                "fence_i", pegasus::Action::createAction<&RvzifenceiInsts::fence_iHandler_<RV64>,
                                                         RvzifenceiInsts>(nullptr, "fence_i",
                                                                          ActionTags::EXECUTE_TAG));
        }
        else if constexpr (std::is_same_v<XLEN, RV32>)
        {
            inst_handlers.emplace(
                "fence_i", pegasus::Action::createAction<&RvzifenceiInsts::fence_iHandler_<RV32>,
                                                         RvzifenceiInsts>(nullptr, "fence_i",
                                                                          ActionTags::EXECUTE_TAG));
        }
    }

    template void RvzifenceiInsts::getInstHandlers<RV32>(std::map<std::string, Action> &);
    template void RvzifenceiInsts::getInstHandlers<RV64>(std::map<std::string, Action> &);

    template <typename XLEN>
    Action::ItrType RvzifenceiInsts::fence_iHandler_(pegasus::PegasusState* state,
                                                     Action::ItrType action_it)
    {
        // TODO: Flush any TLBs and instruction/block caches in the future
        (void)state;

        return ++action_it;
    }

} // namespace pegasus
