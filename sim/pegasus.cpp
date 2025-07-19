
#include <iomanip>

#include "sim/PegasusSim.hpp"
#include "sparta/app/CommandLineSimulator.hpp"

const char USAGE[] = "Usage:\n"
                     "./pegasus [-i inst limit] [--reg \"name value\"] [--interactive] "
                     "[--spike-formatting] <workload>"
                     "\n";

struct RegOverride
{
    std::string name;
    std::string value;
};

inline std::ostream & operator<<(std::ostream & os, RegOverride const & v)
{
    return os << "RegOverride {" << v.name << ", " << v.value << "}";
}

static inline std::istream & operator>>(std::istream & is, RegOverride & into)
{
    return is >> std::skipws >> into.name >> into.value;
}

int main(int argc, char** argv)
{
    uint64_t ilimit = 0;
    std::string workload;
    std::string eot_mode;

    sparta::app::DefaultValues DEFAULTS;
    DEFAULTS.auto_summary_default = "off";
    DEFAULTS.arch_arg_default = "default";
    DEFAULTS.arch_search_dirs = {"arch"}; // Where --arch will be resolved by default

    sparta::SimulationInfo::getInstance() =
        sparta::SimulationInfo("Pegasus RISC-V Functional Model", argc, argv, "v0.0.0", "", {});
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
            ("reg", po::value<std::vector<RegOverride>>()->multitoken(), "Override initial value of a register")
            ("interactive", "Enable interactive mode (IDE)")
            ("eot-mode", po::value<std::string>(&eot_mode), "End of testing mode (pass_fail, magic_mem) [currently IGNORED]")
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

        const auto & vm = cls.getVariablesMap();

        if (0 == vm.count("no-run"))
        {
            if (workload.empty())
            {
                std::cout << "ERROR: Missing a workload to run. Provide an ELF or JSON to run"
                          << std::endl;
                std::cout << USAGE;
                return 1;
            }
        }
        // Workload command line arguments
        std::vector<std::string> workload_args;
        sparta::utils::tokenize_on_whitespace(workload, workload_args);

        // Shove some register overrides in
        pegasus::PegasusSim::RegValueOverridePairs reg_value_overrides;
        if (vm.count("reg"))
        {
            auto & reg_overrides = vm["reg"].as<std::vector<RegOverride>>();
            for (auto reg_override : reg_overrides)
            {
                reg_value_overrides.emplace_back(
                    std::make_pair(reg_override.name, reg_override.value));
            }
        }

        // Create the simulator
        sparta::Scheduler scheduler;
        pegasus::PegasusSim sim(&scheduler, workload_args, reg_value_overrides, ilimit);

        cls.populateSimulation(&sim);

        if (vm.count("interactive"))
        {
            sim.enableInteractiveMode();
        }

        if (vm.count("spike-formatting") > 0)
        {
            sim.useSpikeFormatting();
        }

        if (not eot_mode.empty())
        {
            sim.setEOTMode(eot_mode);
        }

        cls.runSimulator(&sim);

        cls.postProcess(&sim);

        // Get workload exit code
        const pegasus::PegasusState::SimState* sim_state = sim.getPegasusState()->getSimState();
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
