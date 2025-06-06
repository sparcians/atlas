project(logger)

file (CREATE_LINK ${PROJECT_SOURCE_DIR}/../../arch                     ${CMAKE_CURRENT_BINARY_DIR}/arch SYMBOLIC)
file (CREATE_LINK ${PROJECT_SOURCE_DIR}/../../mavis/json               ${CMAKE_CURRENT_BINARY_DIR}/mavis_json SYMBOLIC)
file (CREATE_LINK ${PROJECT_SOURCE_DIR}/workloads                      ${CMAKE_CURRENT_BINARY_DIR}/workloads SYMBOLIC)

add_executable(STFLogger STFLogger_test.cpp) 

target_link_libraries(STFLogger atlascore atlasinsts softfloat atlassys atlassim ${ATLAS_LIBS} ${STF_LINK_LIBS})