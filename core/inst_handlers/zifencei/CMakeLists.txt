project(Pegasus)

file(GLOB SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
add_library(rvzifencei
    OBJECT
    ${SOURCES}
)
add_dependencies(rvzifencei AutogenArchFiles)
