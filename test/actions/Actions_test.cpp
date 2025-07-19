#include "TestComponents.hpp"
#include "sim/PegasusSim.hpp"

#include "sparta/utils/SpartaTester.hpp"

void runSim(pegasus::PegasusState* state, pegasus::ActionGroup* pegasus_core,
            const uint32_t expected_num_insts, const uint32_t expected_num_action_groups)
{
    state->setPc(0x1000);
    state->getSimState()->inst_count = 0;

    pegasus::ActionGroup* action_group = pegasus_core;
    uint32_t num_action_groups_executed = 0;
    while (action_group)
    {
        action_group = action_group->execute(state);
        ++num_action_groups_executed;
    }

    EXPECT_EQUAL(expected_num_insts, state->getSimState()->inst_count);
    EXPECT_EQUAL(expected_num_action_groups, num_action_groups_executed);

    // Reset for next test
    state->getSimState()->inst_count = 0;
}

class dummyClass
{
  public:
    using base_type = dummyClass;

    pegasus::Action::ItrType dummyAction(pegasus::PegasusState*, pegasus::Action::ItrType action_it)
    {
        return ++action_it;
    }
};

int main()
{
    // Create the simulator
    sparta::Scheduler scheduler_;
    const uint64_t ilimit = 0;
    pegasus::PegasusSim pegasus_sim{&scheduler_, {}, {}, ilimit};

    pegasus_sim.buildTree();
    pegasus_sim.configureTree();
    pegasus_sim.finalizeTree();

    pegasus::PegasusState* state = pegasus_sim.getPegasusState();

    // Create core units
    FetchUnit fetch_unit{};
    pegasus::ActionGroup* fetch = fetch_unit.getActionGroup();
    DecodeUnit decode_unit{};
    pegasus::ActionGroup* decode = decode_unit.getActionGroup();
    ExecuteUnit execute_unit{fetch};
    pegasus::ActionGroup* execute = execute_unit.getActionGroup();

    pegasus::Action fetch_action = pegasus::Action::createAction<&FetchUnit::fetch_inst>(
        &fetch_unit, fetch_unit.getName().c_str());
    fetch_action.addTag(pegasus::ActionTags::FETCH_TAG);
    pegasus::Action decode_action = pegasus::Action::createAction<&DecodeUnit::decode_inst>(
        &decode_unit, decode_unit.getName().c_str());
    decode_action.addTag(pegasus::ActionTags::DECODE_TAG);
    pegasus::Action execute_action = pegasus::Action::createAction<&ExecuteUnit::execute_inst>(
        &execute_unit, execute_unit.getName().c_str());
    execute_action.addTag(pegasus::ActionTags::EXECUTE_TAG);

    fetch->addAction(fetch_action);
    decode->addAction(decode_action);
    execute->addAction(execute_action);

    // Connect the ActionGroups together
    fetch->setNextActionGroup(decode);
    decode->setNextActionGroup(execute);
    execute->setNextActionGroup(fetch);

    // Dummy Action
    const std::string dummy_action_name = "Dummy";
    dummyClass dummy_class;
    pegasus::Action dummy_action = pegasus::Action::createAction<&dummyClass::dummyAction>(
        &dummy_class, dummy_action_name.c_str());
    const pegasus::ActionTagType DUMMY_TAG = pegasus::ActionTagFactory::createTag("DUMMY_TAG");
    dummy_action.addTag(DUMMY_TAG);

    //
    // Test the basics, run until wfi
    //
    std::cout << "TEST: Basics\n";
    std::cout << fetch << std::endl;
    std::cout << decode << std::endl;
    std::cout << execute << std::endl;
    std::cout << std::endl;
    // 5 insts + wfi, 4 ActionGroups each + stop sim
    runSim(state, fetch, 6, 25);

    //
    // Add a dummy Action after Fetch
    //
    fetch->insertActionAfter(dummy_action, pegasus::ActionTags::FETCH_TAG);
    std::cout << "TEST: Insert Action After\n";
    std::cout << fetch << std::endl;
    std::cout << std::endl;
    runSim(state, fetch, 6, 25);

    //
    // Add another dummy Action before Execute
    //
    execute->insertActionBefore(dummy_action, pegasus::ActionTags::EXECUTE_TAG);
    std::cout << "TEST: Insert Action After\n";
    std::cout << execute << std::endl;
    std::cout << std::endl;
    runSim(state, fetch, 6, 25);

    //
    // Remove the dummy Actions that were added
    //
    std::cout << "TEST: Remove Actions\n";
    fetch->removeAction(DUMMY_TAG);
    execute->removeAction(DUMMY_TAG);
    std::cout << fetch << std::endl;
    std::cout << decode << std::endl;
    std::cout << execute << std::endl;
    runSim(state, fetch, 6, 25);

    //
    // Trying to remove an Action that is not in the ActionGroup will fail
    //
    std::cout << "TEST: Remove Action Fails\n";
    EXPECT_THROW(decode->removeAction(DUMMY_TAG));

    //
    // Bypass the Decode ActionGroup
    //
    //
    fetch->setNextActionGroup(execute);
    std::cout << "TEST: Bypass ActionGroup\n";
    std::cout << std::endl;
    runSim(state, fetch, 6, 19);
    fetch->setNextActionGroup(decode);

    //
    // Create another ActionGroup for Translation
    //
    std::cout << "TEST: Add Translation ActionGroup\n";
    std::cout << std::endl;
    TranslateUnit translate_unit;
    pegasus::ActionGroup* translate = translate_unit.getActionGroup();
    pegasus::Action translate_action =
        pegasus::Action::createAction<&TranslateUnit::translate_addr>(
            &translate_unit, translate_unit.getName().c_str());
    translate_action.addTag(pegasus::ActionTags::INST_TRANSLATE_TAG);
    translate->addAction(translate_action);
    // fetch -> translate -> decode
    fetch->setNextActionGroup(translate);
    translate->setNextActionGroup(decode);
    std::cout << fetch << std::endl;
    std::cout << translate << std::endl;
    std::cout << decode << std::endl;
    std::cout << execute << std::endl;
    std::cout << std::endl;
    runSim(state, fetch, 6, 31);

    //
    // Configure Execute for Vector Instruction Execution
    //
    const bool vector_wkld = true;
    fetch_unit.loadWorkload(vector_wkld);
    // each vadd.vv counts as 2 insts + wfi
    runSim(state, fetch, 11, 31);

    REPORT_ERROR;
    return ERROR_CODE;
}
