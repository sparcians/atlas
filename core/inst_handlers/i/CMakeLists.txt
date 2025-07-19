project(Pegasus)

file(GLOB SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
add_library(rvi
    OBJECT
    ${SOURCES}
)
add_dependencies(rvi AutogenArchFiles)
