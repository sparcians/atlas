project(Utils_Test)

add_executable(ByteVector_test ByteVector_test.cpp)
target_link_libraries(ByteVector_test pegasuscore pegasusinsts softfloat ${PEGASUS_LIBS})

pegasus_named_test(ByteVector_test_run ByteVector_test)
