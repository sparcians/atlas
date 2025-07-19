project(Translate_Test)

file (CREATE_LINK ${PROJECT_SOURCE_DIR}/../../../arch                     ${CMAKE_CURRENT_BINARY_DIR}/arch SYMBOLIC)
file (CREATE_LINK ${PROJECT_SOURCE_DIR}/../../../mavis/json               ${CMAKE_CURRENT_BINARY_DIR}/mavis_json SYMBOLIC)
file (CREATE_LINK ${PROJECT_SOURCE_DIR}/../../../core/inst_handlers/rv64  ${CMAKE_CURRENT_BINARY_DIR}/rv64 SYMBOLIC)

add_executable(Translate_test Translate_test.cpp)
target_link_libraries(Translate_test pegasuscore pegasusinsts softfloat pegasussys pegasussim ${PEGASUS_LIBS})

pegasus_named_test(Translate_test_run Translate_test)
