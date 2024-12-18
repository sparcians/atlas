/* Test: nop.elf
 * ISA: rv64i
 * Description: Stream of nops.
 */

.macro test_pass
    li x1, 1
    la x2, tohost
    sw x1, 0(x2)
    wfi
.endm

.macro test_fail
    li x1, 2
    la x2, tohost
    sw x1, 0(x2)
    wfi
.endm

.macro start_tracepoint
     xor x0, x0, x0
.endm

.macro stop_tracepoint
     xor x0, x1, x1
.endm
