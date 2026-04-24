#include <stdint.h>
#include "drivers/uart/uart.h"
#include "arch/riscv64/trap.h"
#include "lib/common.h"
#include "mm/pmm.h"
#include "mm/vm.h"

void kernel_main(void)
{
    uart_init();
    println("Eidos is alive.");
    pmm_init();
    vm_init();

    for (;;)
    {
        asm volatile("wfi");
    }
}