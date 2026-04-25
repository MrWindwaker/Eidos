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
    asm volatile("csrs mstatus, %0" : : "r"(1 << 3));
    for (;;)
    {
        printf("A\n");
        if (need_yield)
        {
            need_yield = 0;
            yield();
        }
    }
}

static void proc_b(void)
{
    asm volatile("csrs mstatus, %0" : : "r"(1 << 3));
    for (;;)
    {
        printf("B\n");
        if (need_yield)
        {
            need_yield = 0;
            yield();
        }
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

    uint64_t *sp = (uint64_t *)procs[0].sp;
    printf("proc1 stack[0] (ra): %x\n", sp[0]);
    printf("proc1 stack[1] (s0): %x\n", sp[1]);
    printf("proc1 sp: %x\n", procs[0].sp);

    uint64_t trap_top = (uint64_t)procs[0].trap_stack + KERNEL_STACK_SIZE;
    asm volatile("csrw mscratch, %0" : : "r"(trap_top));

    clint_init();

    asm volatile("csrc mstatus, %0" : : "r"(1 << 3));

    current_proc = NULL;
    yield();

    panic("kernel_main: yield returned");
}