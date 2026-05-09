#include "proc.h"
#include "../mm/pmm.h"
#include "../mm/vm.h"
#include "../lib/common.h"
#include "../../build/userspace/hello.h"

proc_t procs[PROC_MAX];
proc_t *current_proc = NULL;
static uint64_t idle_sp;

#define USER_BASE 0x1000
#define USER_MAP_SIZE 0x20000
#define USER_STACK 0x80000

proc_t *proc_create_user(const uint8_t *binary, uint32_t size)
{
    for (int i = 0; i < PROC_MAX; i++)
    {
        if (procs[i].state == PROC_UNUSED)
        {
            proc_t *p = &procs[i];
            p->pid = i + 1;
            p->state = PROC_RUNNABLE;
            p->pagetable = vm_create();

            uint64_t map_start = USER_BASE;
            uint64_t map_end = 0x13000;
            uint64_t total_pages = (map_end - map_start) / PAGE_SIZE;

            for (uint64_t pg = 0; pg < total_pages; pg++)
            {
                void *page = alloc_page();
                uint64_t va = map_start + pg * PAGE_SIZE;

                uint64_t offset = pg * PAGE_SIZE;
                if (offset < size)
                {
                    uint32_t chunk = size - offset;
                    if (chunk > PAGE_SIZE)
                        chunk = PAGE_SIZE;
                    uint8_t *dst = (uint8_t *)page;
                    for (uint32_t b = 0; b < chunk; b++)
                        dst[b] = binary[offset + b];
                }

                uint64_t flags = PTE_R | PTE_W | PTE_X | PTE_U;
                vm_map(*p->pagetable, va, (uint64_t)page, PAGE_SIZE, flags);
            }

            uint64_t *sp = (uint64_t *)(p->kernel_stack + KERNEL_STACK_SIZE);
            sp -= 13;
            for (int j = 0; j < 13; j++)
                sp[j] = 0;
            sp[0] = (uint64_t)USER_BASE;
            p->sp = (uint64_t)sp;

            p->uentry = USER_BASE;
            p->usp = 0x12360;

            printf("proc: created user process %d entry=%x\n", p->pid, USER_BASE);
            return p;
        }
    }

    panic("proc: no free process slots");
    return NULL;
}

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
        int idx = (start + i) % PROC_MAX;
        if (&procs[idx] != current_proc && procs[idx].state == PROC_RUNNABLE)
        {
            next = &procs[idx];
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

    if (next->uentry != 0)
    {
        uint64_t entry = next->uentry;
        uint64_t usp = next->usp;
        next->uentry = 0;

        asm volatile(
            "csrw mepc, %0\n"
            "csrr t0, mstatus\n"
            "li t1, ~(3<<11)\n"
            "and t0, t0, t1\n"
            "ori t0, t0, (1<<7)\n"
            "csrw mstatus, t0\n"
            "mv sp, %1\n"
            "mret\n" : : "r"(entry), "r"(usp) : "t0", "t1", "memory");

        __builtin_unreachable();
    }

    if (prev)
        proc_switch(&prev->sp, &next->sp);
    else
        proc_switch(&idle_sp, &next->sp);

    asm volatile("csrs mstatus, %0" : : "r"(1 << 3));
}