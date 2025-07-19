#include "PegasusSim.hpp"
#include "include/ActionTags.hpp"
#include "include/CSRFieldIdxs64.hpp"
#include <filesystem>

#include "sparta/utils/LogUtils.hpp"

namespace pegasus
{
    PegasusSim::PegasusSim(sparta::Scheduler* scheduler,
                           const WorkloadAndArguments & workload_and_args,
                           const RegValueOverridePairs & reg_value_overrides, uint64_t ilimit) :
        sparta::app::Simulation("PegasusSim", scheduler),
        workload_and_args_(workload_and_args),
        reg_value_overrides_(reg_value_overrides),
        ilimit_(ilimit)
    {
    }

    PegasusSim::~PegasusSim()
    {
        for (auto state : state_)
        {
            state->cleanup();
        }

        getRoot()->enterTeardown();
    }

    void PegasusSim::run(uint64_t run_time)
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
        const PegasusState* state = state_.at(hart_id);
        const uint64_t inst_count = state->getSimState()->inst_count;
        std::locale::global(std::locale(""));
        std::cout.imbue(std::locale());
        std::cout.precision(12);
        std::cout << "Instructions executed: " << std::dec << inst_count << std::endl;
        std::cout << "Raw time (seconds): " << std::dec << (sim_time / 1000000.0) << std::endl;
        std::cout << "MIPS: " << std::dec << (inst_count / (sim_time / 1000000.0)) << std::endl;

        // TODO: mem usage, workload exit code
    }

    void PegasusSim::setEOTMode(const std::string & eot_mode)
    {
        if (eot_mode == "pass_fail")
        {
            sparta::notNull(system_)->enableEOTPassFailMode();
        }
        else
        {
            sparta_assert(false, "Unknown EOT mode: " << eot_mode << " expected pass_fail");
        }
    }

    void PegasusSim::enableInteractiveMode()
    {
        sparta_assert(!state_.empty(), "Must call after bindTree_()");
        for (auto state : state_)
        {
            state->enableInteractiveMode();
        }
    }

    void PegasusSim::useSpikeFormatting()
    {
        sparta_assert(!state_.empty(), "Must call after bindTree_()");
        for (auto state : state_)
        {
            state->useSpikeFormatting();
        }
    }

    void PegasusSim::buildTree_()
    {
        auto root_tn = getRoot();

        // top.allocators
        allocators_tn_.reset(new PegasusAllocators(getRoot()));

        // top.system
        tns_to_delete_.emplace_back(
            new sparta::ResourceTreeNode(root_tn, "system", "Pegasus System", &system_factory_));

        // top.system_call_emulator
        tns_to_delete_.emplace_back(new sparta::ResourceTreeNode(
            root_tn, "system_call_emulator", "System Call Emulator", &sys_call_factory_));

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

    void PegasusSim::configureTree_()
    {
        // Set PegasusSystem workload parameter
        auto system_workload_and_args =
            getRoot()->getChildAs<sparta::ParameterBase>("system.params.workload_and_args");
        system_workload_and_args->setValueFromStringVector(workload_and_args_);
    }

    void PegasusSim::bindTree_()
    {
        // Pegasus System (shared by all harts)
        system_ = getRoot()->getChild("system")->getResourceAs<pegasus::PegasusSystem>();
        SystemCallEmulator* system_call_emulator =
            getRoot()
                ->getChild("system_call_emulator")
                ->getResourceAs<pegasus::SystemCallEmulator>();

        bool system_call_emulator_enabled = false;
        const uint32_t num_harts = 1;
        for (uint32_t hart_id = 0; hart_id < num_harts; ++hart_id)
        {
            // Get PegasusState and Fetch for each hart
            const std::string core_name = "core" + std::to_string(hart_id);
            const auto core = getRoot()->getChild(core_name);
            state_.emplace_back(core->getResourceAs<PegasusState*>());

            // Give PegasusState a pointer to PegasusSystem for accessing memory
            PegasusState* state = state_.back();
            state->setPegasusSystem(system_);
            state->setPc(system_->getStartingPc());

            if (core->getChildAs<sparta::ResourceTreeNode>("execute")
                    ->getParameterSet()
                    ->getParameterAs<bool>("enable_syscall_emulation"))
            {
                system_call_emulator_enabled = true;
                state->setSystemCallEmulator(system_call_emulator);
            }

            for (const auto & reg_override : reg_value_overrides_)
            {
                const bool MUST_EXIST = true;
                sparta::Register* reg = state->findRegister(reg_override.first, MUST_EXIST);
                const uint64_t new_reg_value = std::stoull(reg_override.second, nullptr, 0);
                uint64_t old_value = 0;
                if (state->getXlen() == 64)
                {
                    old_value = reg->dmiRead<uint64_t>();
                    reg->dmiWrite<uint64_t>(new_reg_value);
                }
                else
                {
                    old_value = reg->dmiRead<uint32_t>();
                    reg->dmiWrite<uint32_t>(new_reg_value);
                }
                std::cout << std::hex << std::showbase << std::setfill(' ');

                std::cout << "Setting " << std::quoted(reg->getName());
                if (reg->getAliases().size() != 0)
                {
                    std::cout << " aka " << reg->getAliases();
                }
                std::cout << " to " << HEX16(new_reg_value) << " from default " << HEX16(old_value)
                          << std::endl;

                std::cout << "Fields:";

                if (not reg->getFields().empty())
                {
                    uint64_t max_field_val_size = 0;
                    size_t max_field_name_size = 0;

                    // Order the fields based on bit settings
                    struct OrderFields
                    {
                        bool operator()(const sparta::RegisterBase::Field* lhs,
                                        const sparta::RegisterBase::Field* rhs) const
                        {
                            return (lhs->getLowBit() > rhs->getLowBit());
                        }
                    };

                    // Go through the fields twice -- once to get formatting, the second to actually
                    // print
                    std::set<sparta::RegisterBase::Field*, OrderFields> ordered_fields;
                    for (const auto & field : reg->getFields())
                    {
                        max_field_val_size = std::max(field->getNumBits(), max_field_val_size);
                        max_field_name_size =
                            std::max(field->getName().size(), max_field_name_size);
                        ordered_fields.insert(field);
                    }
                    max_field_val_size =
                        (max_field_val_size > 4) ? max_field_val_size / 4 : max_field_val_size;
                    max_field_val_size += 2;

                    for (const auto & field : ordered_fields)
                    {
                        std::cout << "\n\t" << std::setw(max_field_val_size) << std::right
                                  << std::dec << field->read() << " "
                                  << std::setw(max_field_name_size) << std::right
                                  << field->getName() << std::right << std::dec << " ["
                                  << std::setw(2) << field->getLowBit() << ":" << std::setw(2)
                                  << field->getHighBit() << "] "
                                  << (field->isReadOnly() ? "RO" : "RW") << " (" << field->getDesc()
                                  << ")";
                    }
                }
                else
                {
                    std::cout << " None";
                }
                std::cout << std::endl;
            }

            if (false == workload_and_args_.empty())
            {
                state->setupProgramStack(system_->getWorkloadAndArgs());
            }
        }
        if (false == workload_and_args_.empty() && system_call_emulator_enabled)
        {
            system_call_emulator->setWorkload(workload_and_args_[0]);
        }
    }

    void PegasusSim::endSimulation(int64_t exit_code)
    {
        for (auto state : state_)
        {
            state->stopSim(exit_code);
        }
    }

} // namespace pegasus
