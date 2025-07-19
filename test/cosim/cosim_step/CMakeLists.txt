project(CoSimStep_Test)

add_executable(CoSimStep_test CoSimStep_test.cpp)
target_link_libraries(CoSimStep_test pegasuscosimlib)

file (CREATE_LINK ${CMAKE_SOURCE_DIR}/arch          ${CMAKE_CURRENT_BINARY_DIR}/arch SYMBOLIC)
file (CREATE_LINK ${CMAKE_SOURCE_DIR}/mavis/json ${CMAKE_CURRENT_BINARY_DIR}/mavis_json SYMBOLIC)
file (CREATE_LINK ${CMAKE_SOURCE_DIR}/core/rv64  ${CMAKE_CURRENT_BINARY_DIR}/rv64 SYMBOLIC)

pegasus_named_test(CoSimStep_test_run CoSimStep_test)
