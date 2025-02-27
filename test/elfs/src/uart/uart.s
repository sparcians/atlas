/* Test: nop.elf
 * ISA: rv64i
 * Description: Stream of nops.
 */

.include "host.s"
.include "macros.s"

.section .text
    .global main

main:

    /* Set UART address */
    li x1, 0x2000
    slli x1, x1, 16

    /* H */
    li x2, 0x48
    sw x2, 0(x1)
    /* e */
    li x2, 0x65
    sw x2, 0(x1)
    /* l */
    li x2, 0x6c
    sw x2, 0(x1)
    /* l */
    li x2, 0x6c
    sw x2, 0(x1)
    /* o */
    li x2, 0x6f
    sw x2, 0(x1)

    /* space */
    li x2, 0x20
    sw x2, 0(x1)

    /* W */
    li x2, 0x57
    sw x2, 0(x1)
    /* o */
    li x2, 0x6f
    sw x2, 0(x1)
    /* r */
    li x2, 0x72
    sw x2, 0(x1)
    /* l */
    li x2, 0x6c
    sw x2, 0(x1)
    /* d */
    li x2, 0x64
    sw x2, 0(x1)

    /* ! */
    li x2, 0x21
    sw x2, 0(x1)

    /* new line */
    li x2, 0xa
    sw x2, 0(x1)

pass:
    test_pass

fail:
    test_fail

.section .data
data:
    .fill 64, 4, 0xFFFFFFFF
