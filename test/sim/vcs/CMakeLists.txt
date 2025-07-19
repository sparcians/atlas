project(Vcs_Test)

file (CREATE_LINK ${PROJECT_SOURCE_DIR}/../../../arch                     ${CMAKE_CURRENT_BINARY_DIR}/arch SYMBOLIC)
file (CREATE_LINK ${PROJECT_SOURCE_DIR}/../../../mavis/json               ${CMAKE_CURRENT_BINARY_DIR}/mavis_json SYMBOLIC)
file (CREATE_LINK ${PROJECT_SOURCE_DIR}/../../../core/inst_handlers/rv64  ${CMAKE_CURRENT_BINARY_DIR}/rv64 SYMBOLIC)

add_executable(Vcs_test Vcs_test.cpp)
target_link_libraries(Vcs_test pegasuscore pegasusinsts softfloat pegasussys pegasussim ${PEGASUS_LIBS})

pegasus_named_test(Vcs_test_run Vcs_test)
