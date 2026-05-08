.section .text
.global _start

_start:
   li a7, 1
   li a0, 1
   la a1, msg
   li a2, 13
   ecall

    li a7, 2
    li a0, 0
    ecall

.loop:
    j .loop

msg:
    .string "hello world\n"