project(PegasusRegister_Test)

add_executable(PegasusRegister_test Register_test.cpp)
target_link_libraries(PegasusRegister_test pegasuscore pegasusinsts softfloat ${PEGASUS_LIBS})

pegasus_named_test(Register_test_run PegasusRegister_test)
pegasus_copy(PegasusRegister_test *.json)
pegasus_copy(PegasusRegister_test ../../arch/rv64/reg_*.json)

add_subdirectory(macros)
