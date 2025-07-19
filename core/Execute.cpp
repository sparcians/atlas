#include "core/Execute.hpp"
#include "core/PegasusInst.hpp"
#include "core/PegasusState.hpp"
#include "core/translate/Translate.hpp"
#include "include/ActionTags.hpp"

#include "sparta/utils/LogUtils.hpp"

#include "core/inst_handlers/a/RvaInsts.hpp"
#include "core/inst_handlers/b/RvzbaInsts.hpp"
#include "core/inst_handlers/b/RvzbbInsts.hpp"
#include "core/inst_handlers/b/RvzbcInsts.hpp"
#include "core/inst_handlers/b/RvzbsInsts.hpp"
#include "core/inst_handlers/d/RvdInsts.hpp"
#include "core/inst_handlers/f/RvfInsts.hpp"
#include "core/inst_handlers/i/RviInsts.hpp"
#include "core/inst_handlers/m/RvmInsts.hpp"
#include "core/inst_handlers/zicsr/RvzicsrInsts.hpp"
#include "core/inst_handlers/zifencei/RvzifenceiInsts.hpp"
#include "core/inst_handlers/zicond/RvzicondInsts.hpp"
#include "core/inst_handlers/v/RvvConfigSettingInsts.hpp"
#include "core/inst_handlers/v/RvvIntegerInsts.hpp"
#include "core/inst_handlers/v/RvvLoadStoreInsts.hpp"
#include "core/inst_handlers/v/RvvMaskInsts.hpp"
#include "core/inst_handlers/v/RvvFloatInsts.hpp"

namespace pegasus
{
    Execute::Execute(sparta::TreeNode* execute_node, const ExecuteParameters* p) :
        sparta::Unit(execute_node),
        enable_syscall_emulation_(p->enable_syscall_emulation)
    {
        Action execute_action = pegasus::Action::createAction<&Execute::execute_>(this, "Execute");
        execute_action.addTag(ActionTags::EXECUTE_TAG);
        execute_action_group_.addAction(execute_action);

        // Get RV64 instruction handlers
        RvzbaInsts::getInstHandlers<RV64>(rv64_inst_actions_);
        RvzbbInsts::getInstHandlers<RV64>(rv64_inst_actions_);
        RvzbcInsts::getInstHandlers<RV64>(rv64_inst_actions_);
        RvzbsInsts::getInstHandlers<RV64>(rv64_inst_actions_);
        RviInsts::getInstHandlers<RV64>(rv64_inst_actions_, enable_syscall_emulation_);
        RvmInsts::getInstHandlers<RV64>(rv64_inst_actions_);
        RvaInsts::getInstHandlers<RV64>(rv64_inst_actions_);
        RvfInsts::getInstHandlers<RV64>(rv64_inst_actions_);
        RvdInsts::getInstHandlers<RV64>(rv64_inst_actions_);
        RvzicsrInsts::getInstHandlers<RV64>(rv64_inst_actions_);
        RvzifenceiInsts::getInstHandlers<RV64>(rv64_inst_actions_);
        RvzicondInsts::getInstHandlers<RV64>(rv64_inst_actions_);
        RvvConfigSettingInsts::getInstHandlers<RV64>(rv64_inst_actions_);
        RvvIntegerInsts::getInstHandlers<RV64>(rv64_inst_actions_);
        RvvLoadStoreInsts::getInstHandlers<RV64>(rv64_inst_actions_);
        RvvMaskInsts::getInstHandlers<RV64>(rv64_inst_actions_);
        RvvFloatInsts::getInstHandlers<RV64>(rv64_inst_actions_);

        // Get RV32 instruction handlers
        RvzbaInsts::getInstHandlers<RV32>(rv32_inst_actions_);
        RvzbbInsts::getInstHandlers<RV32>(rv32_inst_actions_);
        RvzbcInsts::getInstHandlers<RV32>(rv32_inst_actions_);
        RvzbsInsts::getInstHandlers<RV32>(rv32_inst_actions_);
        RviInsts::getInstHandlers<RV32>(rv32_inst_actions_, enable_syscall_emulation_);
        RvmInsts::getInstHandlers<RV32>(rv32_inst_actions_);
        RvaInsts::getInstHandlers<RV32>(rv32_inst_actions_);
        RvfInsts::getInstHandlers<RV32>(rv32_inst_actions_);
        RvdInsts::getInstHandlers<RV32>(rv32_inst_actions_);
        RvzicsrInsts::getInstHandlers<RV32>(rv32_inst_actions_);
        RvzifenceiInsts::getInstHandlers<RV32>(rv32_inst_actions_);
        RvzicondInsts::getInstHandlers<RV32>(rv32_inst_actions_);
        RvvConfigSettingInsts::getInstHandlers<RV32>(rv32_inst_actions_);
        RvvIntegerInsts::getInstHandlers<RV32>(rv32_inst_actions_);
        RvvLoadStoreInsts::getInstHandlers<RV32>(rv32_inst_actions_);
        RvvMaskInsts::getInstHandlers<RV32>(rv32_inst_actions_);
        RvvFloatInsts::getInstHandlers<RV32>(rv32_inst_actions_);

        // Get RV64 instruction compute address handlers
        RviInsts::getInstComputeAddressHandlers<RV64>(rv64_inst_compute_address_actions_);
        RvaInsts::getInstComputeAddressHandlers<RV64>(rv64_inst_compute_address_actions_);
        RvfInsts::getInstComputeAddressHandlers<RV64>(rv64_inst_compute_address_actions_);
        RvdInsts::getInstComputeAddressHandlers<RV64>(rv64_inst_compute_address_actions_);
        RvvLoadStoreInsts::getInstComputeAddressHandlers<RV64>(rv64_inst_compute_address_actions_);

        // Get RV32 instruction compute address handlers
        RviInsts::getInstComputeAddressHandlers<RV32>(rv32_inst_compute_address_actions_);
        RvaInsts::getInstComputeAddressHandlers<RV32>(rv32_inst_compute_address_actions_);
        RvfInsts::getInstComputeAddressHandlers<RV32>(rv32_inst_compute_address_actions_);
        RvdInsts::getInstComputeAddressHandlers<RV32>(rv32_inst_compute_address_actions_);
        RvvLoadStoreInsts::getInstComputeAddressHandlers<RV32>(rv32_inst_compute_address_actions_);

        // Get CSR update handlers
        RvzicsrInsts::getCsrUpdateActions<RV64>(rv64_csr_update_actions_);
        RvzicsrInsts::getCsrUpdateActions<RV32>(rv32_csr_update_actions_);
    }

    template const Execute::InstHandlersMap* Execute::getInstHandlersMap<RV64>() const;
    template const Execute::InstHandlersMap* Execute::getInstHandlersMap<RV32>() const;
    template const Execute::InstHandlersMap*
    Execute::getInstComputeAddressHandlersMap<RV64>() const;
    template const Execute::InstHandlersMap*
    Execute::getInstComputeAddressHandlersMap<RV32>() const;

    Action::ItrType Execute::execute_(PegasusState* state, Action::ItrType action_it)
    {
        // Connect instruction to Fetch
        const auto inst = state->getCurrentInst();
        ActionGroup* inst_action_group = inst->getActionGroup();
        inst_action_group->setNextActionGroup(state->getFinishActionGroup());

        // Insert translation Action into instruction's ActionGroup between the compute address
        // handler and the execute handler
        if (inst->isMemoryInst())
        {
            const ActionGroup* translate_action_group =
                inst->isStoreType() ? state->getTranslateUnit()->getStoreTranslateActionGroup()
                                    : state->getTranslateUnit()->getLoadTranslateActionGroup();
            for (auto it = translate_action_group->getActions().rbegin();
                 it != translate_action_group->getActions().rend(); ++it)
            {
                auto & action = *it;
                inst_action_group->insertActionAfter(action, ActionTags::COMPUTE_ADDR_TAG);
            }
        }

        if (inst->writesCsr())
        {
            const CsrUpdateActionsMap* csr_update_actions = (state->getXlen() == 64)
                                                                ? getCsrUpdateActionsMap<RV64>()
                                                                : getCsrUpdateActionsMap<RV32>();
            auto action_it = csr_update_actions->find(inst->getCsr());
            if (action_it != csr_update_actions->end())
            {
                auto & action = action_it->second;
                inst_action_group->insertActionAfter(action, ActionTags::EXECUTE_TAG);
            }
        }

        state->insertExecuteActions(inst_action_group);

        ILOG(inst);

        // Execute the instruction
        execute_action_group_.setNextActionGroup(inst_action_group);
        return ++action_it;
    }
} // namespace pegasus
