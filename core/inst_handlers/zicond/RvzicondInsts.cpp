#include "core/inst_handlers/inst_helpers.hpp"
#include "core/inst_handlers/zicond/RvzicondInsts.hpp"
#include "include/ActionTags.hpp"
#include "core/ActionGroup.hpp"
#include "core/PegasusState.hpp"
#include "core/PegasusInst.hpp"
#include "core/Exception.hpp"
#include "core/Trap.hpp"

extern "C"
{
#include "source/include/softfloat.h"
}

namespace pegasus
{
    template <typename XLEN>
    void RvzicondInsts::getInstHandlers(Execute::InstHandlersMap & inst_handlers)
    {
        static_assert(std::is_same_v<XLEN, RV64> || std::is_same_v<XLEN, RV32>);
        inst_handlers.emplace(
            "czero.eqz",
            pegasus::Action::createAction<&RvzicondInsts::czeroHandler_<XLEN, std::equal_to<XLEN>>,
                                          RvzicondInsts>(nullptr, "czero.eqz",
                                                         ActionTags::EXECUTE_TAG));
        inst_handlers.emplace(
            "czero.nez",
            pegasus::Action::createAction<
                &RvzicondInsts::czeroHandler_<XLEN, std::not_equal_to<XLEN>>, RvzicondInsts>(
                nullptr, "czero.nez", ActionTags::EXECUTE_TAG));
    }

    template void RvzicondInsts::getInstHandlers<RV32>(Execute::InstHandlersMap &);
    template void RvzicondInsts::getInstHandlers<RV64>(Execute::InstHandlersMap &);

    template <typename XLEN, typename OPERATOR>
    Action::ItrType RvzicondInsts::czeroHandler_(pegasus::PegasusState* state,
                                                 Action::ItrType action_it)
    {
        const PegasusInstPtr & inst = state->getCurrentInst();
        const XLEN rs2_val = READ_INT_REG<XLEN>(state, inst->getRs2());

        if (OPERATOR()(rs2_val, 0))
        {
            WRITE_INT_REG<XLEN>(state, inst->getRd(), 0);
        }
        else
        {
            const XLEN rs1_val = READ_INT_REG<XLEN>(state, inst->getRs1());
            WRITE_INT_REG<XLEN>(state, inst->getRd(), rs1_val);
        }

        return ++action_it;
    }
} // namespace pegasus
