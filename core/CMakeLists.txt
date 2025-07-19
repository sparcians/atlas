project(Pegasus)

add_library(pegasuscore
    STATIC
    ActionGroup.cpp
    PegasusState.cpp
    ActionTags.cpp
    Fetch.cpp
    Execute.cpp
    Exception.cpp
    PegasusExtractor.cpp
    PegasusInst.cpp
    translate/Translate.cpp
    observers/Observer.cpp
    observers/CoSimObserver.cpp
    observers/InstructionLogger.cpp
    observers/SimController.cpp
    observers/STFLogger.cpp
    ../arch/RegisterDefnsJSON.cpp

)

target_include_directories(pegasuscore PUBLIC ${CMAKE_BINARY_DIR})

# Tell cmake that stf_lib is a system include directory
# This suppresses warnings from the stf_lib headers
target_include_directories(pegasuscore SYSTEM PUBLIC stf_lib)

add_dependencies(pegasuscore AutogenArchFiles)
add_dependencies(pegasuscore pegasusinsts)
