OUTPUT_ARCH(riscv)
ENTRY(_start)

SECTIONS {
    . = 0x1000;

    .init : {
        *(.init)
    }

    .text : { 
        *(.text*) 
    }

    .rodata : { *(.rodata*) }
    .data : {
        _global_start = .;
        *(.data*) 
        _global_wnd = .;
    }

    .bss : {
        _sbss = .;
        *(.bss*)
        *(COMMON)
        _ebss = .;
    }

    . = ALIGN(16);
    _heap_start = .;
    . += 0x8000;
    _heap_end = .;

    . = ALIGN(16);
    . += 0x1000;
    _stack_top = .;
}