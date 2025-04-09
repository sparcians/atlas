#include "sim/AtlasSim.hpp"
#include "sparta/app/CommandLineSimulator.hpp"

const char USAGE[] = "Usage:\n"
                     "./atlas [-i inst limit] <workload>"
                     "\n";

int main(int argc, char** argv)
{
    uint64_t ilimit = 0;
    std::string workload;

    sparta::app::DefaultValues DEFAULTS;
    DEFAULTS.auto_summary_default = "off";
    DEFAULTS.arch_arg_default = "default";
    DEFAULTS.arch_search_dirs = {"arch"}; // Where --arch will be resolved by default

    sparta::SimulationInfo::getInstance() =
        sparta::SimulationInfo("Atlas RISC-V Functional Model", argc, argv, "v0.0.0", "", {});
    const bool show_field_names = true;
    sparta::SimulationInfo::getInstance().write(std::cout, "# ", "\n", show_field_names);
    std::cout << "# Sparta Version: " << sparta::SimulationInfo::sparta_version << std::endl;

    int exit_code = 0;
    try
    {
        // Helper class for parsing command line arguments, setting up the
        // simulator, and running the simulator. All of the things done by this
        // classs can be done manually if desired. Use the source for the
        // CommandLineSimulator class as a starting point
        sparta::app::CommandLineSimulator cls(USAGE, DEFAULTS);
        namespace po = boost::program_options;

        auto & app_opts = cls.getApplicationOptions();

        // clang-format off
        app_opts.add_options()
            ("inst-limit,i", po::value<uint64_t>(&ilimit),
             "Stop simulation after the instruction limit has been reached")
            ("interactive", "Enable interactive mode (IDE)")
            ("spike-formatting", "Format the Instruction Logger similar to Spike")
            ("workload,w", po::value<std::string>(&workload), "Worklad to run (ELF or JSON)");

        // Add any positional command-line options
        po::positional_options_description & pos_opts = cls.getPositionalOptions();
        pos_opts.add("workload", -1);
        // clang-format on

        // Parse command line options and configure simulator
        int err_code = 0;
        if (!cls.parse(argc, argv, err_code))
        {
            return err_code; // Any errors already printed to cerr
        }

        if (workload.empty())
        {
            std::cout << "ERROR: Missing a workload to run. Provide an ELF or JSON to run"
                      << std::endl;
            std::cout << USAGE;
            return 1;
        }

        const auto & vm = cls.getVariablesMap();

        // Create the simulator
        sparta::Scheduler scheduler;
        atlas::AtlasSim sim(&scheduler, workload, ilimit);

        cls.populateSimulation(&sim);

        if (vm.count("interactive"))
        {
            sim.enableInteractiveMode();
        }

        if (vm.count("spike-formatting") > 0)
        {
            sim.useSpikeFormatting();
        }

        cls.runSimulator(&sim);

        cls.postProcess(&sim);

        // Get workload exit code
        const atlas::AtlasState::SimState* sim_state = sim.getAtlasState()->getSimState();
        exit_code = sim_state->workload_exit_code;
        std::cout << "Workload exit code: " << std::dec << exit_code << std::endl;
    }
    catch (...)
    {
        // Could still handle or log the exception here
        throw;
    }

    return exit_code;
}
