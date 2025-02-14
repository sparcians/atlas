project(AtlasRegister_Test)

add_executable(AtlasRegister_test Register_test.cpp)
target_link_libraries(AtlasRegister_test atlascore atlasinsts softfloat ${SPARTA_LIBS} mavis)

atlas_named_test(Register_test_run AtlasRegister_test)
atlas_copy(AtlasRegister_test *.json)
atlas_copy(AtlasRegister_test ../../arch/rv64/reg_*.json)

add_subdirectory(macros)
