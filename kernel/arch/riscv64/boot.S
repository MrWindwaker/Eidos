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

    la t0, _trap_stack_top
    csrw mscratch, t0

    call kernel_main
.spin:
    wfi
    j .spin
