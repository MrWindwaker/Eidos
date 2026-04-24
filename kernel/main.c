#include <stdint.h>
#include "drivers/uart/uart.h"
#include "arch/riscv64/trap.h"

void kernel_main(void)
{
    uint64_t mscratch_val;
    asm volatile("csrr %0, mscratch" : "=r"(mscratch_val));

    // uart_init();

    if (mscratch_val == 0xDEAD)
    {
        uart_puts("mtvec write FAILED\n");
    }
    else
    {
        uart_puts("mtvec write OK\n");
    }

    uint64_t mtvec_val;
    asm volatile("csrr %0, mtvec" : "=r"(mtvec_val));
    uart_puts("mtvec: ");
    print_hex(mtvec_val);
    uart_puts("\n");

    uart_puts("Eidos is alive.\n");

    uart_puts("Before trap\n");
    asm volatile(".word 0xFFFFFFFF");
    uart_puts("After trap\n");

    for (;;)
    {
        asm volatile("wfi");
    }
}