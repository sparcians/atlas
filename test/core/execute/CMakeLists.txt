project(Execute_Test)

# This line will make sure atlas is built before running the tests
atlas_regress (atlas)

file (CREATE_LINK ${PROJECT_SOURCE_DIR}/../../../arch                     ${CMAKE_CURRENT_BINARY_DIR}/arch SYMBOLIC)
file (CREATE_LINK ${PROJECT_SOURCE_DIR}/../../../mavis/json               ${CMAKE_CURRENT_BINARY_DIR}/mavis_json SYMBOLIC)
file (CREATE_LINK ${PROJECT_SOURCE_DIR}/../../../core/inst_handlers/rv64  ${CMAKE_CURRENT_BINARY_DIR}/rv64 SYMBOLIC)

add_executable(Execute_test Execute_test.cpp)
target_link_libraries(Execute_test atlascore atlasinsts softfloat atlassys atlassim ${SPARTA_LIBS} mavis)

atlas_named_test(Execute_test_run Execute_test)
