#include "TestComponents.hpp"
#include "sim/AtlasSim.hpp"

#include "sparta/utils/SpartaTester.hpp"

void runSim(atlas::AtlasState* state, atlas::ActionGroup* atlas_core,
            const uint32_t expected_num_insts, const uint32_t expected_num_action_groups)
{
    state->setPc(0x1000);
    state->getSimState()->inst_count = 0;

    atlas::ActionGroup* action_group = atlas_core;
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

    atlas::Action::ItrType dummyAction(atlas::AtlasState*, atlas::Action::ItrType action_it)
    {
        return ++action_it;
    }
};

int main()
{
    // Create the simulator
    sparta::Scheduler scheduler_;
    const uint64_t ilimit = 0;
    atlas::AtlasSim atlas_sim {&scheduler_, {}, {}, ilimit};

    atlas_sim.buildTree();
    atlas_sim.configureTree();
    atlas_sim.finalizeTree();

    atlas::AtlasState* state = atlas_sim.getAtlasState();

    // Create core units
    FetchUnit fetch_unit{};
    atlas::ActionGroup* fetch = fetch_unit.getActionGroup();
    DecodeUnit decode_unit{};
    atlas::ActionGroup* decode = decode_unit.getActionGroup();
    ExecuteUnit execute_unit{fetch};
    atlas::ActionGroup* execute = execute_unit.getActionGroup();

    atlas::Action fetch_action = atlas::Action::createAction<&FetchUnit::fetch_inst>(
        &fetch_unit, fetch_unit.getName().c_str());
    fetch_action.addTag(atlas::ActionTags::FETCH_TAG);
    atlas::Action decode_action = atlas::Action::createAction<&DecodeUnit::decode_inst>(
        &decode_unit, decode_unit.getName().c_str());
    decode_action.addTag(atlas::ActionTags::DECODE_TAG);
    atlas::Action execute_action = atlas::Action::createAction<&ExecuteUnit::execute_inst>(
        &execute_unit, execute_unit.getName().c_str());
    execute_action.addTag(atlas::ActionTags::EXECUTE_TAG);

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
    atlas::Action dummy_action = atlas::Action::createAction<&dummyClass::dummyAction>(&dummy_class, dummy_action_name.c_str());
    const atlas::ActionTagType DUMMY_TAG = atlas::ActionTagFactory::createTag("DUMMY_TAG");
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
    fetch->insertActionAfter(dummy_action, atlas::ActionTags::FETCH_TAG);
    std::cout << "TEST: Insert Action After\n";
    std::cout << fetch << std::endl;
    std::cout << std::endl;
    runSim(state, fetch, 6, 25);

    //
    // Add another dummy Action before Execute
    //
    execute->insertActionBefore(dummy_action, atlas::ActionTags::EXECUTE_TAG);
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
    atlas::ActionGroup* translate = translate_unit.getActionGroup();
    atlas::Action translate_action = atlas::Action::createAction<&TranslateUnit::translate_addr>(
        &translate_unit, translate_unit.getName().c_str());
    translate_action.addTag(atlas::ActionTags::INST_TRANSLATE_TAG);
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
