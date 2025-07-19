#pragma once

#include <vector>
#include <string>
#include <cinttypes>

#include "core/PegasusState.hpp"
#include "core/Fetch.hpp"
#include "core/translate/Translate.hpp"
#include "core/Execute.hpp"
#include "core/Exception.hpp"
#include "system/PegasusSystem.hpp"
#include "sparta/app/Simulation.hpp"
#include "system/SystemCallEmulator.hpp"

namespace pegasus
{
    class PegasusSim : public sparta::app::Simulation
    {
      public:
        using RegValueOverridePairs = std::vector<std::pair<std::string, std::string>>;
        using WorkloadAndArguments = std::vector<std::string>;

        PegasusSim(sparta::Scheduler* scheduler, const WorkloadAndArguments & workload_and_args,
                   const RegValueOverridePairs & reg_value_overrides, uint64_t ilimit);
        ~PegasusSim();

        // Run the simulator
        void run(uint64_t run_time) override;

        PegasusState* getPegasusState(HartId hart_id = 0) const { return state_.at(hart_id); }

        PegasusSystem* getPegasusSystem() const { return system_; }

        void enableInteractiveMode();

        void setEOTMode(const std::string & eot_mode);

        void useSpikeFormatting();

        void endSimulation(int64_t exit_code);

      private:
        void buildTree_() override;
        void configureTree_() override;
        void bindTree_() override;

        sparta::ResourceFactory<pegasus::Fetch, pegasus::Fetch::FetchParameters> fetch_factory_;
        sparta::ResourceFactory<pegasus::Translate, pegasus::Translate::TranslateParameters>
            translate_factory_;
        sparta::ResourceFactory<pegasus::Execute, pegasus::Execute::ExecuteParameters>
            execute_factory_;
        sparta::ResourceFactory<pegasus::Exception, pegasus::Exception::ExceptionParameters>
            exception_factory_;
        sparta::ResourceFactory<pegasus::PegasusState,
                                pegasus::PegasusState::PegasusStateParameters>
            state_factory_;
        sparta::ResourceFactory<pegasus::PegasusSystem,
                                pegasus::PegasusSystem::PegasusSystemParameters>
            system_factory_;
        sparta::ResourceFactory<pegasus::SystemCallEmulator,
                                pegasus::SystemCallEmulator::SystemCallEmulatorParameters>
            sys_call_factory_;
        std::unique_ptr<PegasusAllocators> allocators_tn_;
        std::vector<std::unique_ptr<sparta::TreeNode>> tns_to_delete_;

        // Pegasus State for each hart
        std::vector<PegasusState*> state_;

        // Pegasus system
        PegasusSystem* system_ = nullptr;

        const WorkloadAndArguments workload_and_args_;
        const RegValueOverridePairs reg_value_overrides_;
        const uint64_t ilimit_;
        std::shared_ptr<CoSimQuery> cosim_query_;

        friend class PegasusCoSim;
    };
} // namespace pegasus
