OUTPUT_ARCH(riscv)
ENTRY(_start)

MEMORY {
    RAM (rwx) : ORIGIN = 0x80000000, LENGTH = 126M
}

SECTIONS {
    . = 0x80000000;

    .text : {
        *(.text.boot)
        *(.text*)
    } > RAM

    .rodata : {
        *(.rodata*)
    } > RAM

    .data : {
        *(.data*)
    } > RAM

    .bss : {
        _bss_start = .;
        *(.bss*)
        *(COMMON)
        _bss_end = .;
    } > RAM

    .trap_stack : {
        . = ALIGN(16);
        _trap_stack_bottom = .;
        . += 4096;
        _trap_stack_top = .;
    } > RAM

    . = ALIGN(16);
    . += 8192;
    _stack_top = .;
}