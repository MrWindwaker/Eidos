.section .text.boot
.global _start

_start:
    csrw mie, zero
    
    la sp, _stack_top

    la t0, _bss_start
    la t1, _bss_end

.zero_bss:
    bgeu t0, t1, .done_bss
    sd zero, 0(t0)
    addi t0, t0, 8
    j .zero_bss

.done_bss:
    la t0, trap_entry
    csrw mtvec, t0

    csrr t1, mtvec
    bnez t1, .mtvec_ok
    li t1, 0xDEAD
    csrw mscratch, t1
    j .mtvec_ok

.mtvec_ok:
    la t0, _trap_stack_top
    csrw mscratch, t0

    csrr t0, mtvec
    li t1, 0x10000000
.wait1:
    lb t2, 5(t1)
    andi t2, t2, 0x20
    beqz t2, .wait1
    li t2, 'M'
    sb t2, 0(t1)

    bnez t0, .mtvec_set
    li t2, '0'
    j .write_result
.mtvec_set:
    li t2, '1'
.write_result:
.wait2:
    lb t3, 5(t1)
    andi t3, t3, 0x20
    beqz t3, .wait2
    sb t2, 0(t1)
    li t2, '\n'
.wait3:
    lb t3, 5(t1)
    andi t3, t3, 0x20
    beqz t3, .wait3
    sb t2, 0(t1)

    call kernel_main
.spin:
    wfi
    j .spin
