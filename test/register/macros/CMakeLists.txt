project(PegasusRegisterMacros_Test)

add_executable(PegasusRegisterMacros_test RegisterMacros_test.cpp)
target_link_libraries(PegasusRegisterMacros_test pegasussim pegasuscore pegasusinsts softfloat pegasussys ${PEGASUS_LIBS})

pegasus_named_test(RegisterMacros_test_run PegasusRegisterMacros_test)

file (CREATE_LINK ${PROJECT_SOURCE_DIR}/../../../arch                     ${CMAKE_CURRENT_BINARY_DIR}/arch SYMBOLIC)
file (CREATE_LINK ${PROJECT_SOURCE_DIR}/../../../mavis/json               ${CMAKE_CURRENT_BINARY_DIR}/mavis_json SYMBOLIC)
file (CREATE_LINK ${PROJECT_SOURCE_DIR}/../../../core/inst_handlers/rv64  ${CMAKE_CURRENT_BINARY_DIR}/rv64 SYMBOLIC)
