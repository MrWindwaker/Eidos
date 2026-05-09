#include "syscall.h"
#include "syscall_nr.h"
#include "../../lib/common.h"
#include "../../proc/proc.h"
#include "../../drivers/uart/uart.h"
#include "../../mm/vm.h"

static uint64_t sys_write(trap_frame_t *frame)
{
    uint64_t fd = frame->a0;
    uint64_t va = frame->a1;
    uint64_t len = frame->a2;

    (void)fd;

    uint64_t pa = vm_translate(current_proc->pagetable, va);
    if (pa == 0)
    {
        println("sys_write: bad address %x", va);
        return -1;
    }

    const char *s = (const char *)pa;
    for (uint64_t i = 0; i < len; i++)
        uart_putc(s[i]);

    return len;
}

static uint64_t sys_read(trap_frame_t *frame)
{
    uint64_t fd = frame->a0;
    uint64_t va = frame->a1;
    uint64_t len = frame->a2;

    (void)fd;

    uint64_t pa = vm_translate(current_proc->pagetable, va);
    if (pa == 0)
    {
        println("sys_read: bad address %x", va);
        return -1;
    }

    char *buf = (char *)pa;
    buf[0] = uart_getc();
    return 1;
}

static uint64_t sys_exit(trap_frame_t *frame)
{
    uint64_t code = frame->a0;
    printf("proc %d exited with code %d\n", current_proc->pid, (int)code);
    current_proc->state = PROC_DEAD;
    asm volatile("csrs mstatus, %0" : : "r"(1 << 3));
    yield();

    println("kernel: no more process, halting");
    for (;;)
        asm volatile("wfi");

    return 0;
}

static uint64_t sys_getpid(trap_frame_t *frame)
{
    (void)frame;
    return current_proc->pid;
}

static uint64_t sys_yield(trap_frame_t *frame)
{
    (void)frame;
    asm volatile("csrs mstatus, %0" : : "r"(1 << 3));
    yield();
    return 0;
}

typedef uint64_t (*syscall_fn_t)(trap_frame_t *);
static syscall_fn_t syscall_table[] = {
    [SYS_WRITE] = sys_write,
    [SYS_EXIT] = sys_exit,
    [SYS_GETPID] = sys_getpid,
    [SYS_YIELD] = sys_yield,
    [SYS_READ] = sys_read,
};

#define SYSCALL_MAX (sizeof(syscall_table) / sizeof(syscall_table[0]))

void syscall_dispatch(trap_frame_t *frame)
{
    uint64_t num = frame->a7;

    if (num >= SYSCALL_MAX || syscall_table[num] == NULL)
    {
        println("unknown syscall: %d", (int)num);
        frame->a0 = -1;
        return;
    }

    frame->a0 = syscall_table[num](frame);
}