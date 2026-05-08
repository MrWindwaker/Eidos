.section .text
.global _start

_start:
    li a7, 1
    la a0, msg
    li a1, 35
    ecall

    li a7, 2
    li a0, 0
    ecall

.loop:
    j .loop

msg:
    .string "The developer can't read properly\n"