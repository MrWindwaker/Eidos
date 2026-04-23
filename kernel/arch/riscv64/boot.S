.section .text.boot
.global _start

_start:
    csrw mie, zero

    csrr t0, mhartid
    bnez t0, .spin

    la sp, _stack_top

    la t0, _bss_start
    la t1, _bss_end
.zero_bss:
    bgeu t0, t1, .done_bss
    sd zero, 0(t0)
    addi t0, t0, 8
    j .zero_bss
.done_bss:
    call kernel_main
.spin:
    wfi
    j .spin
