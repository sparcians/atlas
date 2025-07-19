project(Pegasus)

file(GLOB SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
add_library(rvb
    OBJECT
    ${SOURCES}
)
add_dependencies(rvb AutogenArchFiles)