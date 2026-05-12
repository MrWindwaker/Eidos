#ifndef RAMDISK_H
#define RAMDISK_H

#include <stdint.h>
#include <stddef.h>

#define EIDAR_MAGIC 0x45494400
#define EIDAR_NAME_LEN 32

typedef struct
{
    char name[EIDAR_NAME_LEN];
    uint32_t size;
    uint32_t offset;
} eidar_entry_t;

typedef struct
{
    uint32_t magic;
    uint32_t num_files;
    eidar_entry_t files[];
} eidar_header_t;

void ramdisk_init(const void *data, uint32_t size);
const void *ramdisk_find(const char *name, uint32_t *size_out);

#endif