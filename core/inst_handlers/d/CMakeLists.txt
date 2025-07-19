project(Pegasus)

file(GLOB SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
add_library(rvd
    OBJECT
    ${SOURCES}
)
add_dependencies(rvd AutogenArchFiles)
