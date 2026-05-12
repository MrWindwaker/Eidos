#include "ramdisk.h"
#include "../lib/common.h"

static const eidar_header_t *rd = NULL;
static uint32_t rd_size = 0;

void ramdisk_init(const void *data, uint32_t size)
{
    rd = (const eidar_header_t *)data;
    rd_size = size;

    if (rd->magic != EIDAR_MAGIC)
        panic("ramdisk: bad magic %x", rd->magic);

    println("ramdisk: %d file(s) loaded", rd->num_files);
    for (uint32_t i = 0; i < rd->num_files; i++)
        printf("  [%d] %s (%d bytes)\n", i, rd->files[i].name, rd->files[i].size);
}

const void *ramdisk_find(const char *name, uint32_t *size_out)
{
    if (rd == NULL)
        panic("ramdisk: not initialized");

    for (uint32_t i = 0; i < rd->num_files; i++)
    {
        const eidar_entry_t *e = &rd->files[i];

        int match = 1;
        for (int j = 0; j < EIDAR_NAME_LEN; j++)
        {
            if (e->name[j] != name[j])
            {
                match = 0;
                break;
            }
            if (e->name[j] == '\0')
                break;
        }

        if (match)
        {
            if (size_out)
                *size_out = e->size;
            return (const uint8_t *)rd + e->offset;
        }
    }

    return NULL;
}