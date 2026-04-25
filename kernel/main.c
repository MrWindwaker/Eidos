#include <stdint.h>
#include "drivers/uart/uart.h"
#include "arch/riscv64/trap.h"
#include "lib/common.h"
#include "mm/pmm.h"
#include "mm/vm.h"
#include "proc/proc.h"
#include "drivers/clint/clint.h"

static void proc_a(void)
{
    for (;;)
    {
        printf("A\n");
        yield();
    }
}

static void proc_b(void)
{
    for (;;)
    {
        printf("B\n");
        yield();
    }
}

void kernel_main(void)
{
    uart_init();
    println("Eidos is alive.");
    pmm_init();
    vm_init();
    proc_init();

    proc_create(proc_a);
    proc_create(proc_b);

    clint_init();

    current_proc = NULL;
    yield();

    panic("kernel_main: yield returned");
}