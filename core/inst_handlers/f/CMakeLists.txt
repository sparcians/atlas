project(Pegasus)

file(GLOB SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
add_library(rvf
    OBJECT
    ${SOURCES}
)
add_dependencies(rvf AutogenArchFiles)
