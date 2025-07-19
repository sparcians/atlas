project(Pegasus)

file(GLOB SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
add_library(rva
    OBJECT
    ${SOURCES}
)
add_dependencies(rva AutogenArchFiles)
