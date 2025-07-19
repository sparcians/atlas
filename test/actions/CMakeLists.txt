project(Actions_Test)

file (CREATE_LINK ${PROJECT_SOURCE_DIR}/../../arch                     ${CMAKE_CURRENT_BINARY_DIR}/arch SYMBOLIC)
file (CREATE_LINK ${PROJECT_SOURCE_DIR}/../../mavis/json               ${CMAKE_CURRENT_BINARY_DIR}/mavis_json SYMBOLIC)

add_executable(Actions_test Actions_test.cpp)
target_link_libraries(Actions_test pegasuscore pegasusinsts softfloat pegasussys pegasussim ${PEGASUS_LIBS})

pegasus_named_test(Actions_test_run Actions_test)
