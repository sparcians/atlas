#!/usr/bin/env python3

import os
import re
import argparse
import subprocess

def get_tests(directory):
    regex = re.compile(r'rv[36][24]')
    tests = []

    for root, dirs, files in os.walk(directory):
        for file in files:
            if regex.match(file) and "dump" not in file:
                tests.append(os.path.abspath(os.path.join(root, file)))
    tests.sort()
    return tests

SUPPORTED_EXTENSIONS = ["mi", "si", "ui", "um", "ua", "uf", "ud"]

def main():
    parser = argparse.ArgumentParser(description="Script to run the RISC-V architecture tests on Atlas")
    parser.add_argument("xlen", type=str, choices=['rv32', 'rv64'], help="The XLEN value (rv32 or rv64)")
    parser.add_argument("directory", type=str, help="The directory of the built RISC-V architecture tests i.e. riscv-tests/isa")
    args = parser.parse_args()

    tests = get_tests(args.directory)
    if args.xlen == "rv32":
        tests = [test for test in tests if "rv32" in test]
    elif args.xlen == "rv64":
        tests = [test for test in tests if "rv64" in test]

    # TODO: Atlas does not support translation yet
    print("WARNING: Skipping translation tests")
    tests = [test for test in tests if "-p-" in test]
    # TODO: Atlas does not support zba, zbb, zbc, zbs or zfh extensions
    print("WARNING: Skipping all non-G extension tests")
    tests = [test for test in tests if any(ext+"-" in test for ext in SUPPORTED_EXTENSIONS)]

    import multiprocessing
    passing_tests = multiprocessing.Queue()
    failing_tests = multiprocessing.Queue()

    # Function to run a single test and append to the appropriate queue
    def run_test(test, passing_tests, failing_tests):
        testname = os.path.basename(test)
        logname = testname + ".log"
        instlogname = testname + ".instlog"
        atlas_cmd = ["./atlas", "-l", "top", "inst", instlogname, test]
        test_passed = False
        try:
            with open(logname, "w") as f:
                result = subprocess.run(atlas_cmd, stdout=f, stderr=f, timeout=10)
                if result.returncode == 0:
                    test_passed = True
        except subprocess.TimeoutExpired:
            return

        if test_passed:
            passing_tests.put(testname)
            # Remove log files if test passed
            os.remove(logname)
            os.remove(instlogname)
        else:
            failing_tests.put(testname)

    # Function to run tests using processes
    def run_tests_in_parallel(tests, passing_tests, failing_tests):
        print("Running " + str(len(tests)) + " arch tests...")
        processes = []

        # Create a process for each test command
        for test in tests:
            process = multiprocessing.Process(target=run_test, args=(test, passing_tests, failing_tests))
            process.start()
            processes.append(process)

        # Wait for all processes to finish
        for process in processes:
            process.join()

    run_tests_in_parallel(tests, passing_tests, failing_tests)

    num_passed = 0
    print("PASSED:")
    while not passing_tests.empty():
        print("\t" + passing_tests.get())
        num_passed += 1

    print("FAILED:")
    while not failing_tests.empty():
        print("\t" + failing_tests.get())

    print("\nPASS RATE: " + str(num_passed) + "/" + str(len(tests)))


if __name__ == "__main__":
    main()
