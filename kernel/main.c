#include <stdint.h>
#include "drivers/uart/uart.h"
#include "arch/riscv64/trap.h"
#include "lib/common.h"

void kernel_main(void)
{
    uart_init();
    println("Eidos is alive.");

    asm volatile(".word 0xFFFFFFFF");

    for (;;)
    {
        asm volatile("wfi");
    }
}