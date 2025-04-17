#!/usr/bin/env python3

import os, sys
import re
import argparse
import subprocess
import multiprocessing

# Passing and total
PASSING_STATUS_RV32_NON_VIRT = [91, 97]
PASSING_STATUS_RV32_VIRT     = [0, 78]
PASSING_STATUS_RV64_NON_VIRT = [124, 127]
PASSING_STATUS_RV64_VIRT     = [104, 106]

def get_tests(directory):
    regex = re.compile(r'rv[36][24]')
    tests = []

    for root, dirs, files in os.walk(directory):
        for file in files:
            if regex.match(file) and "dump" not in file:
                tests.append(os.path.abspath(os.path.join(root, file)))
    tests.sort()
    return tests

# Function to run a single test and append to the appropriate queue
def run_test(test, passing_tests, failing_tests, timeout_tests, output_dir):
    testname = os.path.basename(test)
    logname = output_dir + testname + ".log"
    instlogname = output_dir + testname + ".instlog"
    error_dump = output_dir + testname + ".error"

    rv32_test = "rv32" in test

    isa_string = "rv32gc_zicsr_zifencei" if rv32_test else "rv64gc_zicsr_zifencei"
    atlas_cmd = ["./atlas", "-l", "top", "inst", instlogname,
                 "--debug-dump-filename", error_dump,
                 "-p", "top.core0.params.isa_string", isa_string, test]
    test_passed = False
    try:
        with open(logname, "w") as f:
            result = subprocess.run(atlas_cmd, stdout=f, stderr=f, timeout=10)
            if result.returncode == 0:
                test_passed = True

    except subprocess.TimeoutExpired:
        timeout_tests.put(testname)
        return

    if test_passed:
        passing_tests.put(testname)
        # Remove log files if test passed
        os.remove(logname)
        os.remove(instlogname)
    else:
        error = 'UNKNOWN'
        with open(logname, "r") as log:
            for line in log:
                if line.startswith('MAGIC') or line.startswith('FAIL'):
                    error = line.strip()
                    break

        failing_tests.put(testname + ' : ' + error)

# Function to run tests using processes
def run_tests_in_parallel(tests, passing_tests, failing_tests, timeout_tests, output_dir):
    print("Running " + str(len(tests)) + " arch tests...")
    processes = []

    # Create a process for each test command
    for test in tests:
        process = multiprocessing.Process(target=run_test, args=(test, passing_tests, failing_tests, timeout_tests, output_dir))
        process.start()
        processes.append(process)

    # Wait for all processes to finish
    for process in processes:
        process.join()


def main():
    parser = argparse.ArgumentParser(description="Script to run the RISC-V architecture tests on Atlas")
    parser.add_argument("directory", type=str, help="The directory of the built RISC-V architecture tests i.e. riscv-tests/isa")
    parser.add_argument("--extensions", type=str, nargs="+", help="The extensions to test (mi, si, ui, um, ua, uf, ud)")
    parser.add_argument("--output", type=str, nargs=1, help="Where logs and error files should go")
    parser.add_argument("--v-only", action='store_true', help="Only run virtual tests (skip baremetal)")
    parser.add_argument("--p-only", action='store_true', help="Only run baremetal tests (skip virtual)")
    parser.add_argument("--rv32-only", action='store_true', help="Only run RV32 tests (skip RV64)")
    parser.add_argument("--rv64-only", action='store_true', help="Only run RV64 tests (skip RV32)")
    args = parser.parse_args()

    SUPPORTED_EXTENSIONS = ["mi", "si", "ui", "um", "ua", "uf", "ud", "uc"]
    if args.extensions:
        assert all([ext in SUPPORTED_EXTENSIONS for ext in args.extensions]), "Unsupported extension(s) provided"
        SUPPORTED_EXTENSIONS = args.extensions

    output_dir = "./"
    if args.output:
        output_dir = args.output[0]
        try:
            os.makedirs(output_dir, exist_ok=True)
            output_dir += '/'
        except Exception as e:
            print("ERROR: Could not make output directory", output_dir, e)
            sys.exit(1)

    tests = get_tests(args.directory)

    if args.rv32_only and args.rv64_only:
        print("ERROR: Cannot set both \'--rv32-only\' and \'--rv64-only\'")
        sys.exit(1)

    if args.p_only and args.v_only:
        print("ERROR: Cannot set both \'--p-only\' and \'--v-only\'")
        sys.exit(1)


    if args.rv32_only:
        print("WARNING: Skipping RV64 tests")
        tests = [test for test in tests if "rv32" in test]

    if args.rv64_only:
        print("WARNING: Skipping RV32 tests")
        tests = [test for test in tests if "rv64" in test]

    if args.p_only:
        print("WARNING: Skipping translation tests")
        tests = [test for test in tests if "-p-" in test]

    if args.v_only:
        print("WARNING: Skipping baremetal tests")
        tests = [test for test in tests if "-v-" in test]

    # TODO: Atlas does not support zba, zbb, zbc, zbs or zfh extensions
    print("WARNING: Skipping all non-G extension tests")
    tests = [test for test in tests if any(ext+"-" in test for ext in SUPPORTED_EXTENSIONS)]

    # Ignore some tests that are unsupported or bad tests
    skip_tests = [
        "rv64uf-p-fclass",     # Atlas does not support the fclass instruction
        "rv64uf-v-fclass",
        "rv32uf-p-fclass",
        "rv32uf-v-fclass",
        "rv64ud-p-fclass",
        "rv64ud-v-fclass",
        "rv32ud-p-fclass",
        "rv32ud-v-fclass",
        "rv64mi-p-breakpoint", # Atlas does not support external debug support
        "rv64mi-v-breakpoint",
        "rv32mi-p-breakpoint",
        "rv32mi-v-breakpoint",
        "rv64mi-p-access",     # BAD TEST: Has check for max paddr size restriction which has been lifted
        "rv64mi-v-access",
    ]
    for skip_test in skip_tests:
        print("Skipping", skip_test)
        tests = [test for test in tests if skip_test not in test]

    import multiprocessing
    passing_tests = multiprocessing.Queue()
    failing_tests = multiprocessing.Queue()
    timeout_tests = multiprocessing.Queue()

    run_tests_in_parallel(tests, passing_tests, failing_tests, timeout_tests, output_dir)

    num_passed = 0
    print("PASSED:")
    while not passing_tests.empty():
        print("\t" + passing_tests.get())
        num_passed += 1

    print("FAILED:")
    while not failing_tests.empty():
        print("\t" + failing_tests.get())

    if not timeout_tests.empty():
        print("TIMED OUT:")
        while not timeout_tests.empty():
            print("\t" + timeout_tests.get())

    print("\n    PASS RATE: " + str(num_passed) + "/" + str(len(tests)))

    expected_passing_tests = 0
    total_tests = 0

    # Get expected pass rate
    if not args.rv64_only:
        if not args.v_only:
            expected_passing_tests += PASSING_STATUS_RV32_NON_VIRT[0]
            total_tests += PASSING_STATUS_RV32_NON_VIRT[1]
        if not args.p_only:
            expected_passing_tests += PASSING_STATUS_RV32_VIRT[0]
            total_tests += PASSING_STATUS_RV32_VIRT[1]
    if not args.rv32_only:
        if not args.v_only:
            expected_passing_tests += PASSING_STATUS_RV64_NON_VIRT[0]
            total_tests += PASSING_STATUS_RV64_NON_VIRT[1]
        if not args.p_only:
            expected_passing_tests += PASSING_STATUS_RV64_VIRT[0]
            total_tests += PASSING_STATUS_RV64_VIRT[1]

    print("EXPECTED RATE: " + str(expected_passing_tests) + "/" + str(total_tests))
    if (num_passed < expected_passing_tests):
        print("ERROR: failed!")

if __name__ == "__main__":
    main()
