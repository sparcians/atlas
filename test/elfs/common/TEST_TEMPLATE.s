/* Test: [test name]
 * ISA: [isa string required]
 * Description: [description of test]
 */

.include "host.s"
.include "macros.s"

.section .text
    .global main

// Insert your test in main
main:

// Jump or fall through here if test is successful
pass:
    test_pass

// Jump here if test is unsuccessful
fail:
    test_fail

.section .data
data:
    .fill 64, 4, 0xFFFFFFFF
