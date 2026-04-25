#include "proc.h"
#include "../mm/pmm.h"
#include "../mm/vm.h"
#include "../lib/common.h"

proc_t procs[PROC_MAX];
proc_t *current_proc = NULL;
static uint64_t idle_sp;

volatile int need_yield = 0;

void proc_init(void)
{
    for (int i = 0; i < PROC_MAX; i++)
    {
        procs[i].pid = 0;
        procs[i].state = PROC_UNUSED;
    }
    println("proc: process table initialized (%d slots)", PROC_MAX);
}

proc_t *proc_create(void (*entry)(void))
{
    for (int i = 0; i < PROC_MAX; i++)
    {
        if (procs[i].state == PROC_UNUSED)
        {
            proc_t *p = &procs[i];

            p->pid = i + 1;
            p->state = PROC_RUNNABLE;
            p->pagetable = vm_create();

            uint64_t *sp = (uint64_t *)(p->kernel_stack + KERNEL_STACK_SIZE);

            sp -= 13;

            for (int j = 0; j < 13; j++)
                sp[j] = 0;

            sp[0] = (uint64_t)entry;

            p->sp = (uint64_t)sp;
            printf("proc: created process %d entry=%x sp=%x\n", p->pid, (uint64_t)entry, p->sp);

            return p;
        }
    }

    panic("pron: no free process slots");
    return NULL;
}

void yield(void)
{
    asm volatile("csrc mstatus, %0" : : "r"(1 << 3));

    int start = 0;
    if (current_proc != NULL)
        start = (current_proc->pid) % PROC_MAX;

    proc_t *next = NULL;
    for (int i = 0; i < PROC_MAX; i++)
    {
        if (&procs[i] != current_proc && procs[i].state == PROC_RUNNABLE)
        {
            next = &procs[i];
            break;
        }
    }

    if (next == NULL)
    {
        asm volatile("csrc mstatus, %0" : : "r"(1 << 3));
        return;
    }

    proc_t *prev = current_proc;
    current_proc = next;
    next->state = PROC_RUNNING;
    if (prev)
        prev->state = PROC_RUNNABLE;

    uint64_t trap_top = (uint64_t)next->trap_stack + KERNEL_STACK_SIZE;
    asm volatile("csrw mscratch, %0" : : "r"(trap_top));

    vm_switch(next->pagetable);

    if (prev)
        proc_switch(&prev->sp, &next->sp);
    else
        proc_switch(&idle_sp, &next->sp);

    asm volatile("csrs mstatus, %0" : : "r"(1 << 3));
}