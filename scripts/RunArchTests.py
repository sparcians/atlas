#!/usr/bin/env python3

import os, sys
import re
import argparse
import subprocess
import multiprocessing
import functools

# Passing and total
PASSING_STATUS_RISCV_ARCH_RV32 = [243, 243]
PASSING_STATUS_RISCV_ARCH_RV64 = [323, 323]
PASSING_STATUS_TENSTORRENT_RV64 = [3999, 3999]

# Verbosity
be_noisy = False

def get_riscv_arch_tests(SUPPORTED_EXTENSIONS, SUPPORTED_XLEN, directory):
    RISCV_ARCH_SUPPORTED_EXTENSIONS = []
    for xlen in SUPPORTED_XLEN:
        RISCV_ARCH_SUPPORTED_EXTENSIONS.extend([xlen+"u"+ext for ext in SUPPORTED_EXTENSIONS])
        if "i" in SUPPORTED_EXTENSIONS:
            RISCV_ARCH_SUPPORTED_EXTENSIONS.extend([xlen+"mi", xlen+"si"])

    regex = re.compile(r'rv[36][24]')
    riscv_arch_tests = []
    for root, dirs, files in os.walk(directory):
        for file in files:
            if regex.match(file) and "dump" not in file:
                riscv_arch_tests.append(os.path.abspath(os.path.join(root, file)))

    riscv_arch_tests = [test for test in riscv_arch_tests if any(ext+"-" in test for ext in RISCV_ARCH_SUPPORTED_EXTENSIONS)]
    riscv_arch_tests.sort()

    tests = []
    for test in riscv_arch_tests:
        testname = test.split("/")[-1]
        tests.append([testname, test]);
    return tests


def get_tenstorrent_tests(SUPPORTED_EXTENSIONS, SUPPORTED_XLEN, directory):
    TENSTORRENT_SUPPORTED_EXTENSIONS = []
    # Tenstorrent tests are all RV64 even if name contains "rv32"
    if "rv64" in SUPPORTED_XLEN:
        TENSTORRENT_SUPPORTED_EXTENSIONS.extend(["rv64"+ext for ext in SUPPORTED_EXTENSIONS])
        TENSTORRENT_SUPPORTED_EXTENSIONS.extend(["rv32"+ext for ext in SUPPORTED_EXTENSIONS])
    else:
        return []

    regex = re.compile(r'rv[36][24]')
    tenstorrent_tests = []
    for root, dirs, files in os.walk(directory):
        for file in files:
            if regex.match(file) and "dump" not in file:
                tenstorrent_tests.append(os.path.abspath(os.path.join(root, file)))
    tenstorrent_tests = [test for test in tenstorrent_tests if any(ext+"_" in test for ext in TENSTORRENT_SUPPORTED_EXTENSIONS)]
    tenstorrent_tests.sort()

    tests = []
    base_dir = "bare_metal"
    
    for test in tenstorrent_tests:
        dirs = test.split('/')
        prefixes = dirs[dirs.index(base_dir)+1:-1]
        prefix = "_".join(prefixes) + "_"
        testname = prefix + os.path.basename(test)
        testname = testname.replace("rv32", "rv64")
        tests.append([testname, test]);
    return tests


# Function to run a single test and append to the appropriate queue
def run_test(testname, wkld, output_dir, passing_tests, failing_tests, timeout_tests):
    if be_noisy:
        print("Running", testname)
    rv32_test = "rv32" in testname
    logname = output_dir + testname + ".log"
    instlogname = output_dir + testname + ".instlog"
    error_dump = output_dir + testname + ".error"
    isa_string = "rv32gcbv_zicsr_zifencei_zicond" if rv32_test else "rv64gcbv_zicsr_zifencei_zicond"
    pegasus_cmd = ["./pegasus",
                 "--debug-dump-filename", error_dump,
                 "-p", "top.core0.params.isa_string", isa_string, wkld]

    test_passed = False
    try:
        with open(logname, "w") as f:
            result = subprocess.run(pegasus_cmd, stdout=f, stderr=f, timeout=10)
            if result.returncode == 0:
                test_passed = True

    except subprocess.TimeoutExpired:
        timeout_tests.append(testname)

    if test_passed:
        # Remove log files if test passed
        os.remove(logname)
        passing_tests.append(testname)
    else:
        error = 'UNKNOWN'
        with open(logname, "r") as log:
            for line in log:
                if line.startswith('MAGIC') or line.startswith('FAIL'):
                    error = line.strip()
                    break

        failing_tests.append([testname, error])


def run_tests_in_parallel(tests, passing_tests, failing_tests, timeout_tests, output_dir):
    import concurrent.futures

    # Limit number of concurrent tests running to the number of CPUs on this machine
    executor = concurrent.futures.ThreadPoolExecutor(max_workers=multiprocessing.cpu_count())
    jobs = []
    for testname, wkld in tests:
        # Submit job
        jobs.append(executor.submit(run_test, testname, wkld, output_dir, passing_tests, failing_tests, timeout_tests))

    # Wait for jobs to complete with a timeout of 5 minutes
    for j in concurrent.futures.as_completed(jobs):
        try:
            result = j.result(timeout=300)
        except TimeoutError:
            print("TIMEOUT:", j)
            continue


def main():
    parser = argparse.ArgumentParser(description="Script to run the Tenstorrent architecture tests on Pegasus")
    parser.add_argument("--extensions", type=str, nargs="+", help="The extensions to test (e.g. i, m, a, f, d)")
    parser.add_argument("--output", type=str, help="Where logs and error files should go")
    xlen_group = parser.add_mutually_exclusive_group()
    xlen_group.add_argument("--rv32-only", action='store_true', help="Only run RV32 tests (skip RV64)")
    xlen_group.add_argument("--rv64-only", action='store_true', help="Only run RV64 tests (skip RV32)")
    parser.add_argument("-v","--verbose", action='store_true', default=False, help="Be noisy")
    parser.add_argument("--riscv-arch", type=str, help="The directory of the built RISC-V Arch tests")
    parser.add_argument("--tenstorrent", type=str, help="The directory of the built Tenstorrent tests")
    args = parser.parse_args()

    global be_noisy

    ###########################################################################
    # Process arguments
    SUPPORTED_EXTENSIONS = ["i", "m", "a", "f", "d", "c", "zba", "zbb", "zbc", "zbs"]
    SUPPORTED_XLEN = ["rv32", "rv64"]
    if args.extensions:
        assert all([ext in SUPPORTED_EXTENSIONS for ext in args.extensions]), "Unsupported extension(s) provided"
        SUPPORTED_EXTENSIONS = args.extensions

    if args.rv32_only:
        print("Skipping RV64 tests")
        SUPPORTED_XLEN = ["rv32"]
    elif args.rv64_only:
        print("Skipping RV32 tests")
        SUPPORTED_XLEN = ["rv64"]

    output_dir = "./"
    if args.output:
        output_dir = args.output
        try:
            os.makedirs(output_dir, exist_ok=True)
            output_dir += '/'
        except Exception as e:
            print("ERROR: Could not make output directory", output_dir, e)
            sys.exit(1)

    if args.rv32_only and args.rv64_only:
        print("ERROR: Cannot set both \'--rv32-only\' and \'--rv64-only\'")
        sys.exit(1)

    be_noisy = args.verbose

    ###########################################################################
    # Make sure we're in the correct sim directory for pegasus
    if not os.path.isfile('./pegasus'):
        print("ERROR: ./pegasus command not found.  Run inside sim directory")
        sys.exit(1)

    ###########################################################################
    # Get tests
    tests = []
    if args.riscv_arch:
        tests.extend(get_riscv_arch_tests(SUPPORTED_EXTENSIONS, SUPPORTED_XLEN, args.riscv_arch))
    if args.tenstorrent:
        tests.extend(get_tenstorrent_tests(SUPPORTED_EXTENSIONS, SUPPORTED_XLEN, args.tenstorrent))

    skip_tests = [
        "rv64mi-p-breakpoint", # Pegasus does not support external debug support
        "rv64mi-v-breakpoint",
        "rv32mi-p-breakpoint",
        "rv32mi-v-breakpoint",
        "rv64mi-p-access",     # BAD TEST: Has check for max paddr size restriction which has been lifted
        "rv64mi-v-access",
    ]
    for skip_test in skip_tests:
        if be_noisy:
            print("Skipping", skip_test)
        tests = [test for test in tests if skip_test not in test]

    if len(tests) == 0:
        print("Failed to find any tests to run! Need to specify --riscv-arch <dir> or --tenstorrent <dir>")
        sys.exit(1)

    ###########################################################################
    # Run!
    print("Running " + str(len(tests)) + " arch tests...")
    passing_tests = []
    failing_tests = []
    timeout_tests = []
    run_tests_in_parallel(tests, passing_tests, failing_tests, timeout_tests, output_dir)

    ###########################################################################
    # Report results
    num_passed = 0
    if passing_tests:
        print("PASSED:")
        for test in passing_tests:
            print("\t" + test)
            num_passed += 1

    if failing_tests:
        print("FAILED:")
        for test,error in failing_tests:
            print("\t" + test + ": " + error)

    if timeout_tests:
        print("TIMED OUT:")
        for test in timeout_tests:
            print("\t" + test)
    print()

    pass_rate = num_passed/len(tests)
    print("PASS RATE: {:.2%} ({}/{})".format(pass_rate, num_passed, len(tests)))

    expected_passing_tests = 0
    total_tests = 0
    if args.riscv_arch:
        if not args.rv64_only:
            expected_passing_tests += PASSING_STATUS_RISCV_ARCH_RV32[0]
            total_tests += PASSING_STATUS_RISCV_ARCH_RV32[1]
        if not args.rv32_only:
            expected_passing_tests += PASSING_STATUS_RISCV_ARCH_RV64[0]
            total_tests += PASSING_STATUS_RISCV_ARCH_RV64[1]
    if args.tenstorrent:
        expected_passing_tests += PASSING_STATUS_TENSTORRENT_RV64[0]
        total_tests += PASSING_STATUS_TENSTORRENT_RV64[1]

    expected_pass_rate = expected_passing_tests/total_tests
    print("EXPECTED RATE: {:.2%} ({}/{})".format(expected_pass_rate, expected_passing_tests, total_tests))
    if (num_passed < expected_passing_tests):
        print("ERROR: failed!")


if __name__ == "__main__":
    main()
