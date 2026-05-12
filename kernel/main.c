#include <stdint.h>
#include "drivers/uart/uart.h"
#include "arch/riscv64/trap.h"
#include "lib/common.h"
#include "mm/pmm.h"
#include "mm/vm.h"
#include "proc/proc.h"
#include "drivers/clint/clint.h"
#include "fs/ramdisk.h"

#include "../build/ramdisk_size.h"

extern const unsigned char _ramdisk_start[];
extern const uint32_t _ramdisk_size;

void kernel_main(void)
{
    uart_init();
    println("Eidos is alive.");
    pmm_init();
    vm_init();
    proc_init();

    ramdisk_init(_ramdisk_start, _ramdisk_size);
    proc_create_user("hello.elf");

    clint_init();

    current_proc = NULL;
    yield();

    panic("kernel_main: yield returned");
}