project(AtlasRegisterMacros_Test)

add_executable(AtlasRegisterMacros_test RegisterMacros_test.cpp)
target_link_libraries(AtlasRegisterMacros_test atlassim atlascore atlasinsts softfloat atlassys ${SPARTA_LIBS} mavis)

atlas_named_test(RegisterMacros_test_run AtlasRegisterMacros_test)

file (CREATE_LINK ${PROJECT_SOURCE_DIR}/../../../arch                     ${CMAKE_CURRENT_BINARY_DIR}/arch SYMBOLIC)
file (CREATE_LINK ${PROJECT_SOURCE_DIR}/../../../mavis/json               ${CMAKE_CURRENT_BINARY_DIR}/mavis_json SYMBOLIC)
file (CREATE_LINK ${PROJECT_SOURCE_DIR}/../../../core/inst_handlers/rv64  ${CMAKE_CURRENT_BINARY_DIR}/rv64 SYMBOLIC)
