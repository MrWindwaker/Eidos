#include "elf.h"
#include "ramdisk.h"
#include "../lib/common.h"
#include "../mm/pmm.h"
#include "../proc/proc.h"

uint64_t elf_load(pagetable_t *pt, const void *elf_data, uint32_t elf_size)
{
    (void)elf_size;
    const Elf64_Ehdr *ehdr = (const Elf64_Ehdr *)elf_data;

    if (ehdr->e_ident[0] != ELF_MAGIC_BYTE0 ||
        ehdr->e_ident[1] != ELF_MAGIC_BYTE1 ||
        ehdr->e_ident[2] != ELF_MAGIC_BYTE2 ||
        ehdr->e_ident[3] != ELF_MAGIC_BYTE3)
        panic("elf: bad magic");
    if (ehdr->e_machine != EM_RISCV)
        panic("elf: not RISC-V");
    if (ehdr->e_type != ET_EXEC)
        panic("elf : not executable");

    const uint8_t *base = (const uint8_t *)elf_data;
    const Elf64_Phdr *phdrs = (const Elf64_Phdr *)(base + ehdr->e_phoff);

    uint64_t highest_va = 0;

    for (uint16_t i = 0; i < ehdr->e_phnum; i++)
    {
        const Elf64_Phdr *ph = &phdrs[i];

        if (ph->p_type != PT_LOAD)
            continue;
        if (ph->p_memsz == 0)
            continue;

        uint64_t flags = PTE_R | PTE_W | PTE_X | PTE_U;

        uint64_t vaddr = ph->p_vaddr & ~(uint64_t)(PAGE_SIZE - 1);
        uint64_t vend = (ph->p_vaddr + ph->p_memsz + PAGE_SIZE - 1) & ~(uint64_t)(PAGE_SIZE - 1);

        if (vend > highest_va)
            highest_va = vend;

        for (uint64_t va = vaddr; va < vend; va += PAGE_SIZE)
        {
            uint64_t existing_pa = vm_translate(pt, va);
            void *page;

            if (existing_pa != 0)
            {
                page = (void *)existing_pa;
                vm_map(*pt, va, (uint64_t)page, PAGE_SIZE, flags);
            }
            else
            {
                page = alloc_page();
                vm_map(*pt, va, (uint64_t)page, PAGE_SIZE, flags);
            }

            uint64_t page_start = va;
            uint64_t seg_start = ph->p_vaddr;
            uint64_t seg_fend = ph->p_vaddr + ph->p_filesz;

            uint64_t copy_start = (seg_start > page_start) ? seg_start : page_start;
            uint64_t copy_end = (seg_fend < page_start + PAGE_SIZE) ? seg_fend : page_start + PAGE_SIZE;

            if (copy_start < copy_end)
            {
                uint64_t dst_off = copy_start - page_start;
                uint64_t src_off = ph->p_offset + (copy_start - seg_start);
                uint64_t len = copy_end - copy_start;

                uint8_t *dst = (uint8_t *)page + dst_off;
                const uint8_t *src = base + src_off;
                for (uint64_t b = 0; b < len; b++)
                    dst[b] = src[b];
            }

            // vm_map(*pt, va, (uint64_t)page, PAGE_SIZE, flags);
        }
    }

    return ehdr->e_entry;
}