#ifndef VM_H
#define VM_H

#include <stdint.h>

#define PTE_V (1 << 0)
#define PTE_R (1 << 1)
#define PTE_W (1 << 2)
#define PTE_X (1 << 3)
#define PTE_U (1 << 4)

#define PAGE_SIZE 4096
#define SATP_SV39 (8ULL << 60)

#define PA_TO_PTE(pa) (((uint64_t)(pa) >> 12) << 10)
#define PTE_TO_PA(pte) (((uint64_t)(pte) >> 10) << 12)

typedef uint64_t pte_t;
typedef pte_t pagetable_t[512];

void vm_init(void);
void vm_map(pagetable_t root, uint64_t vaddr, uint64_t paddr, uint64_t size, uint64_t flags);
void vm_map_kernel(pagetable_t root);
pagetable_t *vm_create(void);
void vm_switch(pagetable_t *pt);

#endif