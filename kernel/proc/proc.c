#include "proc.h"
#include "../mm/pmm.h"
#include "../mm/vm.h"
#include "../lib/common.h"

proc_t procs[PROC_MAX];
proc_t *current_proc = NULL;

static context_t idle_context;

void proc_init(void)
{
    for (int i = 0; i < PROC_MAX; i++)
    {
        procs[i].pid = 0;
        procs[i].state = PROC_UNUSED;
    }
    println("proc: process table initialized ($d slots)", PROC_MAX);
}

proc_t *proc_create(void)
{
    for (int i = 0; i < PROC_MAX; i++)
    {
        if (procs[i].state == PROC_UNUSED)
        {
            proc_t *p = &procs[i];

            p->pid = i + 1;
            p->state = PROC_RUNNABLE;
            p->pagetable = NULL;

            uint64_t sp = (uint64_t)p->kernel_stack + KERNEL_STACK_SIZE;
            p->context.sp = sp;
            p->context.ra = 0;

            println("proc: created process %d", p->pid);
            return p;
        }
    }

    panic("pron: no free process slots");
    return NULL;
}

void yield(void)
{
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
        return;

    proc_t *prev = current_proc;
    current_proc = next;
    next->state = PROC_RUNNING;

    if (prev)
    {
        prev->state = PROC_RUNNABLE;
        proc_switch(&prev->context, &next->context);
    }
    else
    {
        proc_switch(&idle_context, &next->context);
    }
}