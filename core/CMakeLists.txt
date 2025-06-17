project(Atlas)

add_library(atlascore
    STATIC
    ActionGroup.cpp
    AtlasState.cpp
    ActionTags.cpp
    Fetch.cpp
    Execute.cpp
    Exception.cpp
    AtlasExtractor.cpp
    AtlasInst.cpp
    translate/Translate.cpp
    observers/Observer.cpp
    observers/CoSimObserver.cpp
    observers/InstructionLogger.cpp
    observers/SimController.cpp
    ../arch/RegisterDefnsJSON.cpp
    observers/STFtrace/STFLogger.cpp

)

target_include_directories(atlascore PUBLIC ${CMAKE_BINARY_DIR})

# Tell cmake that stf_lib is a system include directory
# This suppresses warnings from the stf_lib headers
target_include_directories(atlascore SYSTEM PUBLIC stf_lib)

add_dependencies(atlascore AutogenArchFiles)
add_dependencies(atlascore atlasinsts)
