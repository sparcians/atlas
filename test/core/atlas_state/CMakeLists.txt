project(AtlasState_Test)

file (CREATE_LINK ${PROJECT_SOURCE_DIR}/../../../arch                     ${CMAKE_CURRENT_BINARY_DIR}/arch SYMBOLIC)
file (CREATE_LINK ${PROJECT_SOURCE_DIR}/../../../mavis/json               ${CMAKE_CURRENT_BINARY_DIR}/mavis_json SYMBOLIC)
file (CREATE_LINK ${PROJECT_SOURCE_DIR}/../../../core/inst_handlers/rv64  ${CMAKE_CURRENT_BINARY_DIR}/rv64 SYMBOLIC)

add_executable(AtlasState_test AtlasState_test.cpp)
target_link_libraries(AtlasState_test atlascore atlasinsts softfloat atlassys atlassim ${SPARTA_LIBS} mavis)

atlas_named_test(AtlasState_test_run AtlasState_test)
