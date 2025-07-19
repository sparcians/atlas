project(Pegasus)

file(GLOB SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
add_library(rvzicond
    OBJECT
    ${SOURCES}
)
add_dependencies(rvzicond AutogenArchFiles)
