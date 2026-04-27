.section .text
.global _start

_start:
    li a7, 1
    la a0, 0
    la a1, 0
    li a2, 13
    ecall

    li a7, 2
    li a0, 0
    ecall

.loop:
    j .loop