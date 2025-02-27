project (BASE_TESTS)

set (CMAKE_ASM_FLAGS "-march=rv64i_zicsr_zifencei")

add_subdirectory (nop)
add_subdirectory (uart)
