
#
# Testing macros as well as setting up Valgrind options
#

# MACROS for adding to the targets. Use these to add your tests.

# atlas_regress enforces that your binary gets built as part of the
# regression commands.
macro (atlas_regress target)
  add_dependencies (atlas_regress ${target} )
  add_dependencies (atlas_regress_valgrind ${target})
endmacro (atlas_regress)

# A function to add a atlas test with various options
function (atlas_fully_named_test name target run_valgrind)
  add_test (NAME ${name} COMMAND $<TARGET_FILE:${target}> ${ARGN})
  atlas_regress (${target})
  # Only add a valgrind test if desired.
  # The older 2.6 version of cmake has issues with this.
  # will ignore them for now.
  if (VALGRIND_REGRESS_ENABLED)
    if (run_valgrind)
          add_test (NAME valgrind_${name} COMMAND valgrind
            ${VALGRIND_OPTS} $<TARGET_FILE:${target}> ${ARGN})
          set_tests_properties (valgrind_${name} PROPERTIES LABELS ${VALGRIND_TEST_LABEL})
    endif ()
  endif ()
endfunction (atlas_fully_named_test)

function (atlas_expect_failure_test name target)
  add_test (NAME ${name} COMMAND $<TARGET_FILE:${target}> ${ARGN})
  set_tests_properties(${name} PROPERTIES WILL_FAIL TRUE)
  atlas_regress (${target})
endfunction (atlas_expect_failure_test)

# Tell atlas to run the following target with the following name.
macro (atlas_named_test name target)
  atlas_fully_named_test (${name} ${target} TRUE ${ARGN})
endmacro (atlas_named_test)

# Run the test without a valgrind test.
# This should only be used for special tests, and not an excuse to avoid
# fixing memory issues!
macro (atlas_named_test_no_valgrind name target)
  atlas_fully_named_test (${name} ${target} FALSE ${ARGN})
endmacro (atlas_named_test_no_valgrind)

# Just add the executable to the testing using defaults.
macro (atlas_test target)
  atlas_named_test (${target} ${target} ${ARGN})
endmacro (atlas_test)

# Define a macro for copying required files to be along side the build
# files.  This is useful for golden outputs in atlas's tests that need
# to be copied to the build directory.
macro (atlas_copy build_target cp_file)
    add_custom_command (TARGET ${build_target} PRE_BUILD
      COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/${cp_file} ${CMAKE_CURRENT_BINARY_DIR}/)
endmacro (atlas_copy)

# Define a macro for recursively copying required files to be along
# side the build files.  This is useful for golden outputs in atlas's
# tests that need to be copied to the build directory.
macro (atlas_recursive_copy build_target cp_file)
    add_custom_command (TARGET ${build_target} PRE_BUILD
      COMMAND cp -r ${CMAKE_CURRENT_SOURCE_DIR}/${cp_file} ${CMAKE_CURRENT_BINARY_DIR}/)
endmacro (atlas_recursive_copy)

#
# Create a atlas test executable -- this is where the linking
# libraries for all of the test are defined.
#
macro (atlas_add_test_executable target)
  add_executable (${target} ${ARGN})
  target_link_libraries(${target} ${Sparta_LIBS})
endmacro (atlas_add_test_executable)
