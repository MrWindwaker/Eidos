.section .text
.global proc_switch

proc_switch:

    addi sp, sp, -13 * 8
    sd ra, 0 * 8(a0)
    sd s0, 1 * 8(a0)
    sd s1, 2 * 8(a0)
    sd s2, 3 * 8(a0)
    sd s3, 4 * 8(a0)
    sd s4, 5 * 8(a0)
    sd s5, 6 * 8(a0)
    sd s6, 7 * 8(a0)
    sd s7, 8 * 8(a0)
    sd s8, 9 * 8(a0)
    sd s9, 10 * 8(a0)
    sd s10, 11 * 8(a0)
    sd s11, 12 * 8(a0)

    sd sp , 0(a0)
    ld sp, 0(a1)

    ld ra, 0 * 8(a0)
    ld s0, 1 * 8(a0)
    ld s1, 2 * 8(a0)
    ld s2, 3 * 8(a0)
    ld s3, 4 * 8(a0)
    ld s4, 5 * 8(a0)
    ld s5, 6 * 8(a0)
    ld s6, 7 * 8(a0)
    ld s7, 8 * 8(a0)
    ld s8, 9 * 8(a0)
    ld s9, 10 * 8(a0)
    ld s10, 11 * 8(a0)
    ld s11, 12 * 8(a0)
    addi sp, sp, 13*8

    ret
