#include "vm.h"
#include "pmm.h"
#include "../lib/common.h"

static pagetable_t kernel_pagetable __attribute__((aligned(PAGE_SIZE)));

static uint64_t vpn(uint64_t vaddr, int level)
{
    return (vaddr >> (12 + level * 9)) & 0x1ff;
}

void vm_map(pagetable_t root, uint64_t vaddr, uint64_t paddr, uint64_t size, uint64_t flags)
{
    uint64_t end = vaddr + size;

    while (vaddr < end)
    {
        pte_t *l2 = &root[vpn(vaddr, 2)];
        if (!(*l2 & PTE_V))
        {
            void *page = alloc_page();
            *l2 = PA_TO_PTE(page) | PTE_V;
        }

        pagetable_t *l1_table = (pagetable_t *)PTE_TO_PA(*l2);
        pte_t *l1 = &(*l1_table)[vpn(vaddr, 1)];
        if (!(*l1 & PTE_V))
        {
            void *page = alloc_page();
            *l1 = PA_TO_PTE(page) | PTE_V;
        }

        pagetable_t *l0_table = (pagetable_t *)PTE_TO_PA(*l1);
        pte_t *l0 = &(*l0_table)[vpn(vaddr, 0)];
        *l0 = PA_TO_PTE(paddr) | flags | PTE_V;

        vaddr += PAGE_SIZE;
        paddr += PAGE_SIZE;
    }
}

void vm_init(void)
{
    extern char _free_ram_end[];
    uint64_t ram_start = 0x80000000;
    uint64_t ram_end = (uint64_t)_free_ram_end;
    uint64_t size = ram_end - ram_start;

    vm_map(kernel_pagetable, ram_start, ram_start, size, PTE_R | PTE_W | PTE_X);
    vm_map(kernel_pagetable, 0x10000000, 0x10000000, PAGE_SIZE, PTE_R | PTE_W);
    vm_map(kernel_pagetable, 0x20000000, 0x20000000, 0x10000, PTE_R | PTE_W);

    uint64_t satp = SATP_SV39 | ((uint64_t)kernel_pagetable >> 12);
    asm volatile("csrw satp, %0" : : "r"(satp));

    asm volatile("sfence.vma zero, zero");

    println("vm: MMU enabled");
}