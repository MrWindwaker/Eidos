#include "trap.h"
#include "../../lib/common.h"
#include "../../drivers/clint/clint.h"
#include "../../proc/proc.h"

static const char *exception_cause(uint64_t cause)
{
    switch (cause)
    {
    case 0:
        return "Instruction address misaligned";
    case 1:
        return "Instruction access fault";
    case 2:
        return "Illegal instruction";
    case 3:
        return "Breakpoint";
    case 4:
        return "Load address misaligned";
    case 5:
        return "Load access fault";
    case 6:
        return "Store/AMO address misaligned";
    case 7:
        return "Store/AMO access fault";
    case 8:
        return "Environment call from U-mode";
    case 9:
        return "Environment call from S-mode";
    case 11:
        return "Environment call from M-mode";
    case 12:
        return "Instruction page fault";
    case 13:
        return "Load page fault";
    case 15:
        return "Store/AMO page fault";
    default:
        return "Unknown exception";
    }
}

static const char *interrupt_cause(uint64_t cause)
{
    switch (cause)
    {
    case 1:
        return "Supervisor software interrupt";
    case 3:
        return "Machine software interrupt";
    case 5:
        return "Supervisor timer interrupt";
    case 7:
        return "Machine timer interrupt";
    case 9:
        return "Supervisor external interrupt";
    case 11:
        return "Machine external interrupt";
    default:
        return "Unknown interrupt";
    }
}

void print_hex(uint64_t val)
{
    printf("0x");
    char buf[17];
    buf[16] = '\0';

    for (int i = 15; i >= 0; i--)
    {
        int nibble = val & 0xF;
        buf[i] = nibble < 10 ? '0' + nibble : 'a' + nibble - 10;
        val >>= 4;
    }
    printf(buf);
}

void trap_handler(trap_frame_t *frame)
{
    (void)frame;
    uint64_t mcause, mepc, mtval;

    asm volatile("csrr %0, mcause" : "=r"(mcause));
    asm volatile("csrr %0, mepc" : "=r"(mepc));
    asm volatile("csrr %0, mtval" : "=r"(mtval));

    uint64_t is_interrupt = mcause >> 63;
    uint64_t cause_code = mcause & ~(1ULL << 63);

    printf("trap! mcause=%x mpec=%x mtval=%x\n", mcause, mepc, mtval);

    if (is_interrupt && cause_code == 7)
    {
        clint_rest_timer();
        need_yield = 1;
        return;
    }

    if (!is_interrupt && cause_code == 8)
    {
        uint64_t syscall_num = frame->a7;

        if (syscall_num == 1)
            println("user: sys_write called");
        else if (syscall_num == 2)
        {
            println("user: sys_exited called");
            current_proc->state = PROC_DEAD;

            asm volatile("csrs mstatus, %0" : : "r"(1 << 3));
            yield();

            println("kernel: no more process, halting");
            for (;;)
                asm volatile("wfi");
        }

        asm volatile("csrr %0, mepc" : "=r"(mepc));
        asm volatile("csrw mepc, %0" : : "r"(mepc + 4));
        return;
    }

    if (is_interrupt)
    {
        println("\nCause: %s", interrupt_cause(cause_code));
    }
    else
    {
        println("\nCause: %s", exception_cause(cause_code));
    }
    panic("\nmepc: %x  mtval: %x\nra:   %x  sp:    %x\n", mepc, mtval, frame->ra, frame->sp);
}