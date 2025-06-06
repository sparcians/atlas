#pragma once

#include <vector>
#include <string>
#include <cinttypes>

#include "core/AtlasState.hpp"
#include "core/Fetch.hpp"
#include "core/translate/Translate.hpp"
#include "core/Execute.hpp"
#include "core/Exception.hpp"
#include "system/AtlasSystem.hpp"
#include "sparta/app/Simulation.hpp"
#include "system/SystemCallEmulator.hpp"

namespace atlas
{
    class AtlasSim : public sparta::app::Simulation
    {
      public:

        using RegValueOverridePairs = std::vector<std::pair<std::string, std::string>>;
        using WorkloadAndArguments = std::vector<std::string>;

        AtlasSim(sparta::Scheduler* scheduler,
                 const WorkloadAndArguments & workload_and_args,
                 const RegValueOverridePairs & reg_value_overrides,
                 uint64_t ilimit);
        ~AtlasSim();

        // Run the simulator
        void run(uint64_t run_time) override;

        AtlasState* getAtlasState(HartId hart_id = 0) const { return state_.at(hart_id); }

        AtlasSystem* getAtlasSystem() const { return system_; }

        void enableInteractiveMode();

        void useSpikeFormatting();

        void endSimulation(int64_t exit_code);

      private:
        void buildTree_() override;
        void configureTree_() override;
        void bindTree_() override;

        sparta::ResourceFactory<atlas::Fetch, atlas::Fetch::FetchParameters> fetch_factory_;
        sparta::ResourceFactory<atlas::Translate, atlas::Translate::TranslateParameters>
            translate_factory_;
        sparta::ResourceFactory<atlas::Execute, atlas::Execute::ExecuteParameters> execute_factory_;
        sparta::ResourceFactory<atlas::Exception, atlas::Exception::ExceptionParameters>
            exception_factory_;
        sparta::ResourceFactory<atlas::AtlasState, atlas::AtlasState::AtlasStateParameters>
            state_factory_;
        sparta::ResourceFactory<atlas::AtlasSystem, atlas::AtlasSystem::AtlasSystemParameters>
            system_factory_;
        sparta::ResourceFactory<atlas::SystemCallEmulator,
                                atlas::SystemCallEmulator::SystemCallEmulatorParameters>
            sys_call_factory_;
        std::unique_ptr<AtlasAllocators> allocators_tn_;
        std::vector<std::unique_ptr<sparta::TreeNode>> tns_to_delete_;

        // Atlas State for each hart
        std::vector<AtlasState*> state_;

        // Atlas system
        AtlasSystem * system_ = nullptr;

        const WorkloadAndArguments workload_and_args_;
        const RegValueOverridePairs reg_value_overrides_;
        const uint64_t ilimit_;
        std::shared_ptr<CoSimQuery> cosim_query_;

        friend class AtlasCoSim;
    };
} // namespace atlas
