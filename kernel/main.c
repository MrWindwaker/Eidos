#include <stdint.h>
#include "drivers/uart/uart.h"
#include "arch/riscv64/trap.h"
#include "lib/common.h"
#include "mm/pmm.h"
#include "mm/vm.h"
#include "proc/proc.h"

static void proc_a(void)
{
    for (;;)
    {
        println("A");
        yield();
    }
}

static void proc_b(void)
{
    for (;;)
    {
        println("B");
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

    proc_t *pa = proc_create();
    pa->context.ra = (uint64_t)proc_a;

    proc_t *pb = proc_create();
    pb->context.ra = (uint64_t)proc_b;

    current_proc = NULL;
    yield();

    panic("kernel_main: yield returned");
}