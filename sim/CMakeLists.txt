project(Atlas)

add_library(atlassim
    STATIC
    AtlasSim.cpp
)

add_dependencies(atlassim AutogenArchFiles)

# Atlas simulator
add_executable(atlas atlas.cpp)

target_link_libraries(atlas atlassim atlascore atlasinsts softfloat atlassys ${ATLAS_LIBS} ${STF_LINK_LIBS})

file (CREATE_LINK ${PROJECT_SOURCE_DIR}/../arch                    ${CMAKE_CURRENT_BINARY_DIR}/arch SYMBOLIC)
file (CREATE_LINK ${PROJECT_SOURCE_DIR}/../mavis/json              ${CMAKE_CURRENT_BINARY_DIR}/mavis_json SYMBOLIC)
