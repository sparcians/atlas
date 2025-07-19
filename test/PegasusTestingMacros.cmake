
#
# Testing macros as well as setting up Valgrind options
#

# MACROS for adding to the targets. Use these to add your tests.

# pegasus_regress enforces that your binary gets built as part of the
# regression commands.
macro (pegasus_regress target)
  add_dependencies (pegasus_regress ${target} )
  add_dependencies (pegasus_regress_valgrind ${target})
endmacro (pegasus_regress)

# A function to add a pegasus test with various options
function (pegasus_fully_named_test name target run_valgrind)
  add_test (NAME ${name} COMMAND $<TARGET_FILE:${target}> ${ARGN})
  pegasus_regress (${target})
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
endfunction (pegasus_fully_named_test)

function (pegasus_expect_failure_test name target)
  add_test (NAME ${name} COMMAND $<TARGET_FILE:${target}> ${ARGN})
  set_tests_properties(${name} PROPERTIES WILL_FAIL TRUE)
  pegasus_regress (${target})
endfunction (pegasus_expect_failure_test)

# Tell pegasus to run the following target with the following name.
macro (pegasus_named_test name target)
  pegasus_fully_named_test (${name} ${target} TRUE ${ARGN})
endmacro (pegasus_named_test)

# Run the test without a valgrind test.
# This should only be used for special tests, and not an excuse to avoid
# fixing memory issues!
macro (pegasus_named_test_no_valgrind name target)
  pegasus_fully_named_test (${name} ${target} FALSE ${ARGN})
endmacro (pegasus_named_test_no_valgrind)

# Just add the executable to the testing using defaults.
macro (pegasus_test target)
  pegasus_named_test (${target} ${target} ${ARGN})
endmacro (pegasus_test)

# Define a macro for copying required files to be along side the build
# files.  This is useful for golden outputs in pegasus's tests that need
# to be copied to the build directory.
macro (pegasus_copy build_target cp_file)
    add_custom_command (TARGET ${build_target} PRE_BUILD
      COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/${cp_file} ${CMAKE_CURRENT_BINARY_DIR}/)
endmacro (pegasus_copy)

# Define a macro for recursively copying required files to be along
# side the build files.  This is useful for golden outputs in pegasus's
# tests that need to be copied to the build directory.
macro (pegasus_recursive_copy build_target cp_file)
    add_custom_command (TARGET ${build_target} PRE_BUILD
      COMMAND cp -r ${CMAKE_CURRENT_SOURCE_DIR}/${cp_file} ${CMAKE_CURRENT_BINARY_DIR}/)
endmacro (pegasus_recursive_copy)

#
# Create a pegasus test executable -- this is where the linking
# libraries for all of the test are defined.
#
macro (pegasus_add_test_executable target)
  add_executable (${target} ${ARGN})
  target_link_libraries(${target} ${Sparta_LIBS})
endmacro (pegasus_add_test_executable)
