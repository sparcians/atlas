project(atlas_test)

# This line will make sure atlas is built before running the tests
atlas_regress (atlas)

file (CREATE_LINK ${PROJECT_SOURCE_DIR}/../../arch                     ${CMAKE_CURRENT_BINARY_DIR}/arch SYMBOLIC)
file (CREATE_LINK ${PROJECT_SOURCE_DIR}/../../mavis/json               ${CMAKE_CURRENT_BINARY_DIR}/mavis_json SYMBOLIC)
file (CREATE_LINK ${PROJECT_SOURCE_DIR}/../../core/inst_handlers/rv64  ${CMAKE_CURRENT_BINARY_DIR}/rv64 SYMBOLIC)
file (CREATE_LINK ${PROJECT_SOURCE_DIR}/workloads                      ${CMAKE_CURRENT_BINARY_DIR}/workloads SYMBOLIC)

# Tests
atlas_named_test(atlas_nop_test atlas -p top.core0.params.stop_sim_on_wfi true workloads/nop.elf)
atlas_named_test(atlas_inst_logger_test atlas -p top.core0.params.stop_sim_on_wfi true -l top inst nop.instlog workloads/nop.elf)
atlas_named_test(atlas_uart_test atlas -p top.core0.params.stop_sim_on_wfi true -p top.system.params.enable_uart true workloads/uart.elf)
