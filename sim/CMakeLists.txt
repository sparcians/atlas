project(Pegasus)

add_library(pegasussim
    STATIC
    PegasusSim.cpp
)

add_dependencies(pegasussim AutogenArchFiles)

# Pegasus simulator
add_executable(pegasus pegasus.cpp)

target_link_libraries(pegasus pegasuscore pegasusinsts softfloat pegasussys pegasussim ${PEGASUS_LIBS})

file (CREATE_LINK ${PROJECT_SOURCE_DIR}/../arch                    ${CMAKE_CURRENT_BINARY_DIR}/arch SYMBOLIC)
file (CREATE_LINK ${PROJECT_SOURCE_DIR}/../mavis/json              ${CMAKE_CURRENT_BINARY_DIR}/mavis_json SYMBOLIC)
