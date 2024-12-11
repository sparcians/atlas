#include "core/Execute.hpp"
#include "core/AtlasInst.hpp"
#include "core/AtlasState.hpp"
#include "core/Translate.hpp"
#include "include/ActionTags.hpp"

#include "sparta/utils/LogUtils.hpp"

#include "core/inst_handlers/a/RvaInsts.hpp"
#include "core/inst_handlers/d/RvdInsts.hpp"
#include "core/inst_handlers/f/RvfInsts.hpp"
#include "core/inst_handlers/i/RviInsts.hpp"
#include "core/inst_handlers/m/RvmInsts.hpp"
#include "core/inst_handlers/zicsr/RvzicsrInsts.hpp"
#include "core/inst_handlers/zifencei/RvzifenceiInsts.hpp"

namespace atlas
{
    Execute::Execute(sparta::TreeNode* execute_node, const ExecuteParameters* p) :
        sparta::Unit(execute_node)
    {
        (void)p;

        Action execute_action = atlas::Action::createAction<&Execute::execute_>(this, "Execute");
        execute_action.addTag(ActionTags::EXECUTE_TAG);
        execute_action_group_.addAction(execute_action);

        // TODO: Get RV32 inst handlers
        // Get instruction handlers
        RviInsts::getInstHandlers<RV64>(inst_handlers_);
        RvmInsts::getInstHandlers<RV64>(inst_handlers_);
        RvaInsts::getInstHandlers<RV64>(inst_handlers_);
        RvfInsts::getInstHandlers<RV64>(inst_handlers_);
        RvdInsts::getInstHandlers<RV64>(inst_handlers_);
        RvzicsrInsts::getInstHandlers<RV64>(inst_handlers_);
        RvzifenceiInsts::getInstHandlers<RV64>(inst_handlers_);

        // Get instruction compute address handlers
        RviInsts::getInstComputeAddressHandlers<RV64>(inst_compute_address_handlers_);
        RvaInsts::getInstComputeAddressHandlers<RV64>(inst_compute_address_handlers_);
        RvfInsts::getInstComputeAddressHandlers<RV64>(inst_compute_address_handlers_);
        RvdInsts::getInstComputeAddressHandlers<RV64>(inst_compute_address_handlers_);
    }

    ActionGroup* Execute::execute_(AtlasState* state)
    {
        // Connect instruction to Fetch
        const auto inst = state->getCurrentInst();
        ActionGroup* inst_action_group = inst->getActionGroup();
        inst_action_group->setNextActionGroup(execute_action_group_.getNextActionGroup());

        // Insert translation Action into instruction's ActionGroup between the compute address
        // handler and the execute handler
        if (inst->isMemoryInst())
        {
            const ActionGroup* data_translate_action_group =
                state->getTranslateUnit()->getDataTranslateActionGroup();
            for (auto it = data_translate_action_group->getActions().rbegin();
                 it != data_translate_action_group->getActions().rend(); ++it)
            {
                auto & action = *it;
                inst_action_group->insertActionAfter(action, ActionTags::COMPUTE_ADDR_TAG);
            }
        }

        state->insertExecuteActions(inst_action_group);

        ILOG(inst);

        // Execute the instruction
        return inst_action_group;
    }
} // namespace atlas
