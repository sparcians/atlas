from backend.observers import *
import argparse

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Pegasus IDE')
    parser.add_argument('--riscv-tests-dir', required=True, help='Path to the RISC-V tests directory')
    parser.add_argument('--sim-exe-path', required=True, help='Path to the RISC-V simulator executable')
    parser.add_argument('--test-name', required=True, help='Name of the test to run')
    args = parser.parse_args()

    obs = SanityCheckObserver('sanity_check.log')
    obs_sim = ObserverSim(args.riscv_tests_dir, args.sim_exe_path, args.test_name)
    report = obs_sim.RunObserver(obs, timeout=10)

    if report:
        report.Show()
