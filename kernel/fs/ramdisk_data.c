__attribute__((section(".ramdisk")))
__attribute__((aligned(4096)))
const unsigned char _ramdisk_blod[] = {
#include "../../build/ramdisk_bytes.h"
};