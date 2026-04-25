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
            p->pagetable = NULL;

            uint64_t *sp = (uint64_t *)(p->kernel_stack + KERNEL_STACK_SIZE);

            sp -= 13;
            sp[0] = (uint64_t)entry;
            sp[1] = 0;
            sp[2] = 0;
            sp[3] = 0;
            sp[4] = 0;
            sp[5] = 0;
            sp[6] = 0;
            sp[7] = 0;
            sp[8] = 0;
            sp[9] = 0;
            sp[10] = 0;
            sp[11] = 0;
            sp[12] = 0;

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
    printf("yield called, current=%d\n", current_proc ? current_proc->pid : -1);

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
        println("yield: no runnable process found");
        return;
    }

    uint64_t trap_top = (uint64_t)next->trap_stack + KERNEL_STACK_SIZE;
    asm volatile("csrw mscratch, %0" : : "r"(trap_top));

    proc_t *prev = current_proc;
    current_proc = next;
    next->state = PROC_RUNNING;

    asm volatile("csrc mstatus, %0" : : "r"(1 << 3));

    if (prev)
    {
        prev->state = PROC_RUNNABLE;
        proc_switch(&prev->sp, &next->sp);
    }
    else
    {
        proc_switch(&idle_sp, &next->sp);
    }

    asm volatile("csrs mstatus, %0" : : "r"(1 << 3));
}