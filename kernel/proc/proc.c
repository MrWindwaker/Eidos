#include "proc.h"
#include "../mm/pmm.h"
#include "../mm/vm.h"
#include "../lib/common.h"
#include "../fs/ramdisk.h"
#include "../fs/elf.h"

proc_t procs[PROC_MAX];
proc_t *current_proc = NULL;
static uint64_t idle_sp;

#define USER_BASE 0x1000
#define USER_MAP_SIZE 0x20000
#define USER_STACK 0x80000
#define USER_STACK_PAGES 4
#define USER_STACK_TOP 0x80000000ULL

proc_t *proc_create_user(const char *name)
{
    for (int i = 0; i < PROC_MAX; i++)
    {
        if (procs[i].state == PROC_UNUSED)
        {
            proc_t *p = &procs[i];
            p->pid = i + 1;
            p->state = PROC_RUNNABLE;
            p->pagetable = vm_create();

            uint32_t elf_size = 0;
            const void *elf_data = ramdisk_find(name, &elf_size);
            if (elf_data == NULL)
                panic("proc; file not found: %s", name);

            uint64_t entry = elf_load(p->pagetable, elf_data, elf_size);

            uint64_t extra_start = 0x2000;
            uint64_t extra_end = 0xb000;

            for (uint64_t va = extra_start; va < extra_end; va += PAGE_SIZE)
            {
                if (vm_translate(p->pagetable, va) != 0)
                    continue;

                void *page = alloc_page();
                vm_map(*p->pagetable, va, (uint64_t)page, PAGE_SIZE, PTE_R | PTE_W | PTE_U);
            }

            uint64_t *sp = (uint64_t *)(p->kernel_stack + KERNEL_STACK_SIZE);
            sp -= 13;
            for (int j = 0; j < 13; j++)
                sp[j] = 0;
            sp[0] = entry;
            p->sp = (uint64_t)sp;

            p->uentry = entry;
            p->usp = 0xa360;

            printf("proc: loaded '%s' entry=%x\n", name, entry);
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