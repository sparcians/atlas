#include "core/inst_handlers/zifencei/RvzifenceiInsts.hpp"
#include "include/ActionTags.hpp"
#include "core/ActionGroup.hpp"
#include "core/AtlasState.hpp"
#include "core/AtlasInst.hpp"

namespace atlas
{
    template <typename XLEN>
    void RvzifenceiInsts::getInstHandlers(std::map<std::string, Action> & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);
        if constexpr (std::is_same_v<XLEN, RV64>)
        {
            inst_handlers.emplace(
                "fence.i", atlas::Action::createAction<&RvzifenceiInsts::fence_i_handler<RV64>,
                                                       RvzifenceiInsts>(nullptr, "fence_i",
                                                                        ActionTags::EXECUTE_TAG));
        }
        else if constexpr (std::is_same_v<XLEN, RV32>)
        {
            inst_handlers.emplace(
                "fence.i", atlas::Action::createAction<&RvzifenceiInsts::fence_i_handler<RV32>,
                                                       RvzifenceiInsts>(nullptr, "fence_i",
                                                                        ActionTags::EXECUTE_TAG));
        }
    }

    template void RvzifenceiInsts::getInstHandlers<RV32>(std::map<std::string, Action> &);
    template void RvzifenceiInsts::getInstHandlers<RV64>(std::map<std::string, Action> &);

    template <typename XLEN> ActionGroup* RvzifenceiInsts::fence_i_handler(atlas::AtlasState* state)
    {
        // TODO: Flush any TLBs and instruction/block caches in the future
        (void)state;

        return nullptr;
    }

} // namespace atlas
