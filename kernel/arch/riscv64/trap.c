#include "trap.h"
#include "../../drivers/uart/uart.h"

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

// Interrupt cause codes (mcause, interrupt bit set)
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
    uart_puts("0x");
    char buf[17];
    buf[16] = '\0';

    for (int i = 15; i >= 0; i--)
    {
        int nibble = val & 0xF;
        buf[i] = nibble < 10 ? '0' + nibble : 'a' + nibble - 10;
        val >>= 4;
    }
    uart_puts(buf);
}

void trap_handler(trap_frame_t *frame)
{
    uart_puts("TRAP FIRED\n");
    uint64_t mcause, mepc, mtval;

    asm volatile("csrr %0, mcause" : "=r"(mcause));
    asm volatile("csrr %0, mepc" : "=r"(mepc));
    asm volatile("csrr %0, mtval" : "=r"(mtval));

    uint64_t is_interrupt = mcause >> 63;
    uint64_t cause_code = mcause & ~(1ULL << 63);

    uart_puts("\n--- EIDOS TRAP ---\n");

    if (is_interrupt)
    {
        uart_puts("Type: Interrupt\n");
        uart_puts("Cause: ");
        uart_puts(interrupt_cause(cause_code));
    }
    else
    {
        uart_puts("Type: Exception\n");
        uart_puts("Cause: ");
        uart_puts(exception_cause(cause_code));
    }

    uart_puts("\nmepc:  ");
    print_hex(mepc);
    uart_puts("\nmtval: ");
    print_hex(mtval);
    uart_puts("\n\nRegisters:\n");
    uart_puts("  ra:  ");
    print_hex(frame->ra);
    uart_puts("  sp:  ");
    print_hex(frame->sp);
    uart_puts("  a0:  ");
    print_hex(frame->a0);
    uart_puts("  a1:  ");
    print_hex(frame->a1);
    uart_puts("\n------------------\n");

    for (;;)
    {
        asm volatile("wfi");
    }
}