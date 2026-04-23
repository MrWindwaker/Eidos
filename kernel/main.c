#include "drivers/uart/uart.h"

void kernel_main(void)
{
    uart_init();
    uart_puts("Eidos is alive.\n");

    for (;;)
    {
        asm volatile("wfi");
    }
}