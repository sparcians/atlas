#include "core/inst_handlers/zifencei/RvzifenceiInsts.hpp"
#include "include/ActionTags.hpp"
#include "core/ActionGroup.hpp"

namespace atlas
{
    template <typename XLEN>
    void RvzifenceiInsts::getInstHandlers(std::map<std::string, Action> & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same<XLEN, RV32>::value);
        if constexpr (std::is_same_v<XLEN, RV64>)
        {
            inst_handlers.emplace(
                "fence.i",
                atlas::Action::createAction<&RvzifenceiInsts::fence_i_64_handler, RvzifenceiInsts>(
                    nullptr, "fence_i", ActionTags::EXECUTE_TAG));
        }
        else if constexpr (std::is_same_v<XLEN, RV32>)
        {
            sparta_assert(false, "RV32 is not supported yet!");
        }
    }

    template void RvzifenceiInsts::getInstHandlers<RV32>(std::map<std::string, Action> &);
    template void RvzifenceiInsts::getInstHandlers<RV64>(std::map<std::string, Action> &);

    ActionGroup* RvzifenceiInsts::fence_i_64_handler(atlas::AtlasState* state)
    {
        (void)state;
        ///////////////////////////////////////////////////////////////////////
        // START OF SPIKE CODE

        // MMU.flush_icache();

        // END OF SPIKE CODE
        ///////////////////////////////////////////////////////////////////////
        return nullptr;
    }

} // namespace atlas
