project(Vred_Test)

file(CREATE_LINK ${PROJECT_SOURCE_DIR}/../../../arch                     ${CMAKE_CURRENT_BINARY_DIR}/arch SYMBOLIC)
file(CREATE_LINK ${PROJECT_SOURCE_DIR}/../../../mavis/json               ${CMAKE_CURRENT_BINARY_DIR}/mavis_json SYMBOLIC)
file(CREATE_LINK ${PROJECT_SOURCE_DIR}/../../../core/inst_handlers/rv64  ${CMAKE_CURRENT_BINARY_DIR}/rv64 SYMBOLIC)

add_executable(Vred_test Vred_test.cpp)

target_link_libraries(Vred_test 
    atlassim
    atlascore
    atlasinsts
    softfloat
    atlassys
    ${ATLAS_LIBS}
)

atlas_named_test(Vred_test_run Vred_test)