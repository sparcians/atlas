#include "AtlasSim.hpp"
#include "include/ActionTags.hpp"
#include "include/CSRFieldIdxs64.hpp"
#include <filesystem>

#include "sparta/utils/LogUtils.hpp"

namespace atlas
{
    AtlasSim::AtlasSim(sparta::Scheduler* scheduler,
                       const WorkloadAndArguments & workload_and_args,
                       const RegValueOverridePairs & reg_value_overrides,
                       uint64_t ilimit) :
        sparta::app::Simulation("AtlasSim", scheduler),
        workload_and_args_(workload_and_args),
        reg_value_overrides_(reg_value_overrides),
        ilimit_(ilimit)
    {
    }

    AtlasSim::~AtlasSim()
    {
        for (auto state : state_)
        {
            state->cleanup();
        }

        getRoot()->enterTeardown();
    }

    void AtlasSim::run(uint64_t run_time)
    {
        for (auto state : state_)
        {
            state->boot();
        }

        getSimulationConfiguration()->scheduler_exacting_run = true;
        getSimulationConfiguration()->scheduler_measure_run_time = false;
        auto start = std::chrono::system_clock::system_clock::now();
        sparta::app::Simulation::run(run_time);
        auto end = std::chrono::system_clock::system_clock::now();
        auto sim_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

        const HartId hart_id = 0;
        const AtlasState* state = state_.at(hart_id);
        const uint64_t inst_count = state->getSimState()->inst_count;
        std::cout << "Instructions executed: " << std::dec << inst_count << std::endl;
        std::cout << "Raw time (seconds): " << std::dec << (sim_time / 1000000.0) << std::endl;
        std::cout << "MIPS: " << std::dec << (inst_count / (sim_time / 1000000.0)) << std::endl;

        // TODO: mem usage, workload exit code
    }

    void AtlasSim::enableInteractiveMode()
    {
        sparta_assert(!state_.empty(), "Must call after bindTree_()");
        for (auto state : state_)
        {
            state->enableInteractiveMode();
        }
    }

    void AtlasSim::useSpikeFormatting()
    {
        sparta_assert(!state_.empty(), "Must call after bindTree_()");
        for (auto state : state_)
        {
            state->useSpikeFormatting();
        }
    }

    void AtlasSim::buildTree_()
    {
        auto root_tn = getRoot();

        // top.allocators
        allocators_tn_.reset(new AtlasAllocators(getRoot()));

        // top.system
        tns_to_delete_.emplace_back(
            new sparta::ResourceTreeNode(root_tn, "system", "Atlas System", &system_factory_));

        // top.system_call_emulator
        tns_to_delete_.emplace_back(
            new sparta::ResourceTreeNode(root_tn, "system_call_emulator", "System Call Emulator",
                                         &sys_call_factory_));

        // top.core
        sparta::TreeNode* core_tn = nullptr;
        const std::string core_name = "core0";
        const uint32_t core_idx = 0;
        tns_to_delete_.emplace_back(
            core_tn = new sparta::ResourceTreeNode(root_tn, core_name, "cores", core_idx,
                                                   "Core State", &state_factory_));

        // top.core.fetch
        tns_to_delete_.emplace_back(new sparta::ResourceTreeNode(
            core_tn, "fetch", sparta::TreeNode::GROUP_NAME_NONE, sparta::TreeNode::GROUP_IDX_NONE,
            "Fetch Unit", &fetch_factory_));

        // top.core.translate
        tns_to_delete_.emplace_back(new sparta::ResourceTreeNode(
            core_tn, "translate", sparta::TreeNode::GROUP_NAME_NONE,
            sparta::TreeNode::GROUP_IDX_NONE, "Translate Unit", &translate_factory_));

        // top.core.execute
        tns_to_delete_.emplace_back(new sparta::ResourceTreeNode(
            core_tn, "execute", sparta::TreeNode::GROUP_NAME_NONE, sparta::TreeNode::GROUP_IDX_NONE,
            "Execute Unit", &execute_factory_));

        // top.core.exception
        tns_to_delete_.emplace_back(new sparta::ResourceTreeNode(
            core_tn, "exception", sparta::TreeNode::GROUP_NAME_NONE,
            sparta::TreeNode::GROUP_IDX_NONE, "Exception Unit", &exception_factory_));
    }

    void AtlasSim::configureTree_()
    {
        // Set AtlasSystem workload parameter
        auto system_workload_and_args =
            getRoot()->getChildAs<sparta::ParameterBase>("system.params.workload_and_args");
        system_workload_and_args->setValueFromStringVector(workload_and_args_);
    }

    void AtlasSim::bindTree_()
    {

        // Atlas System (shared by all harts)
        system_ = getRoot()->getChild("system")->getResourceAs<atlas::AtlasSystem>();
        SystemCallEmulator *system_call_emulator =
            getRoot()->getChild("system_call_emulator")->getResourceAs<atlas::SystemCallEmulator>();

        if (false == workload_and_args_.empty()) {
            system_call_emulator->setWorkload(workload_and_args_[0]);
        }

        const uint32_t num_harts = 1;
        for (uint32_t hart_id = 0; hart_id < num_harts; ++hart_id)
        {
            // Get AtlasState and Fetch for each hart
            const std::string core_name = "core" + std::to_string(hart_id);
            state_.emplace_back(getRoot()->getChild(core_name)->getResourceAs<AtlasState*>());
            // Give AtlasState a pointer to AtlasSystem for accessing memory
            AtlasState* state = state_.back();
            state->setAtlasSystem(system_);
            state->setSystemCallEmulator(system_call_emulator);
            state->setPc(system_->getStartingPc());

            for (const auto & reg_override : reg_value_overrides_)
            {
                const bool MUST_EXIST = true;
                sparta::Register* reg = state->findRegister(reg_override.first, MUST_EXIST);
                const uint64_t reg_value = std::stoull(reg_override.second, nullptr, 0);
                if (state->getXlen() == 64)
                {
                    reg->dmiWrite<uint64_t>(reg_value);
                }
                else
                {
                    reg->dmiWrite<uint32_t>(reg_value);
                }
                std::cout << "Writing " << std::quoted(reg->getName());
                if (reg->getAliases().size() != 0) {
                    std::cout << " aka " << reg->getAliases();
                }
                std::cout << " to " << HEX16(reg_value) << std::endl;
            }

            if (false == workload_and_args_.empty()) {
                state->setupProgramStack(system_->getWorkloadAndArgs());
            }
        }
    }

    void AtlasSim::endSimulation(int64_t exit_code)
    {
        for (auto state : state_) {
            state->stopSim(exit_code);
        }
    }

} // namespace atlas
