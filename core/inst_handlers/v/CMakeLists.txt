project(Pegasus)

file(GLOB SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
add_library(rvv
    OBJECT
    ${SOURCES}
)
add_dependencies(rvv AutogenArchFiles)
