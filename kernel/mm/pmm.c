#include "pmm.h"
#include "../lib/common.h"

extern char _free_ram_start[];
extern char _free_ram_end[];

static void *free_list = NULL;

void pmm_init(void)
{
    char *page = _free_ram_start;

    while (page + PAGE_SIZE <= _free_ram_end)
    {
        free_page(page);
        page += PAGE_SIZE;
    }

    uint64_t total = (_free_ram_end - _free_ram_start) / PAGE_SIZE;
    printf("pmm : %d pages available (%d MB)\n", total, (total * PAGE_SIZE) / (1024 * 1024));
}

void *alloc_page(void)
{
    if (free_list == NULL)
    {
        panic("pmm: out of memory");
    }

    void *page = free_list;
    free_list = *(void **)free_list;

    char *p = (char *)page;
    for (int i = 0; i < PAGE_SIZE; i++)
        p[i] = 0;
    return page;
}

void free_page(void *addr)
{
    *(void **)addr = free_list;
    free_list = addr;
}