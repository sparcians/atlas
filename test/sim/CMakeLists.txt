project(pegasus_test)

# This line will make sure pegasus is built before running the tests
pegasus_regress (pegasus)

file (CREATE_LINK ${PROJECT_SOURCE_DIR}/../../arch                     ${CMAKE_CURRENT_BINARY_DIR}/arch SYMBOLIC)
file (CREATE_LINK ${PROJECT_SOURCE_DIR}/../../mavis/json               ${CMAKE_CURRENT_BINARY_DIR}/mavis_json SYMBOLIC)
file (CREATE_LINK ${PROJECT_SOURCE_DIR}/workloads                      ${CMAKE_CURRENT_BINARY_DIR}/workloads SYMBOLIC)

# Tests
add_subdirectory(vcs)
add_subdirectory(via)
add_subdirectory(vls)
add_subdirectory(vm)
# add_subdirectory(vro)     #TODO: Uncomment when handler support added

# ASM Tests
pegasus_named_test(pegasus_nop_test pegasus -p top.core0.params.stop_sim_on_wfi true workloads/nop.elf)
pegasus_named_test(pegasus_uart_test pegasus -p top.system.params.enable_uart true workloads/uart.elf)

# Linux Tests
set (LINUX_ARCH_SETUP --reg "sp 0x0000003ffffff000"
                      --reg "gp 0x77000"
                      --reg "tp 0x7d000" -p top.core0.execute.params.enable_syscall_emulation true)

set (TEST_TEXT_FILE ${PROJECT_SOURCE_DIR}/../elfs/linux/syscall_test/test_text.txt)
pegasus_named_test(pegasus_dhry_test pegasus ${LINUX_ARCH_SETUP} workloads/dhry.elf)
pegasus_named_test(pegasus_fstatat_test pegasus ${LINUX_ARCH_SETUP} "workloads/fstatat_test.elf ${TEST_TEXT_FILE} 0" )
pegasus_named_test(pegasus_syscall_test pegasus ${LINUX_ARCH_SETUP} "workloads/syscall_test.elf ${TEST_TEXT_FILE}" )

# Logging tests
pegasus_named_test(pegasus_inst_logger_test pegasus -l top inst nop.instlog workloads/nop.elf)
pegasus_named_test(spike_inst_logger_test pegasus -l top inst nop.instlog --spike-formatting workloads/nop.elf)
