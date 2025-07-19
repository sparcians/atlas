project(Pegasus)

add_library(pegasussys
    STATIC
    PegasusSystem.cpp
    SimpleUART.cpp
    MagicMemory.cpp
    SystemCallEmulator.cpp
)
