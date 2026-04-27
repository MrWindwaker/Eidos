#include <stdint.h>
#include "drivers/uart/uart.h"
#include "arch/riscv64/trap.h"
#include "lib/common.h"
#include "mm/pmm.h"
#include "mm/vm.h"
#include "proc/proc.h"
#include "drivers/clint/clint.h"

#include "../build/userspace/hello.h"

void kernel_main(void)
{
    uart_init();
    println("Eidos is alive.");
    pmm_init();
    vm_init();
    proc_init();

    proc_create_user(hello_bin, hello_bin_size);

    clint_init();

    current_proc = NULL;
    yield();

    panic("kernel_main: yield returned");
}