#include "clint.h"
#include "../../lib/common.h"

void clint_rest_timer(void)
{
    CLINT_MTIMECMP = CLINT_MTIME + TIMER_INTERVAL;
}

void clint_init(void)
{
    clint_rest_timer();

    asm volatile("csrs mie, %0" : : "r"(1 << 7));
    asm volatile("csrs mstatus, %0" : : "r"(1 << 3));

    println("clint: timer initialized, interval=%d cycles", TIMER_INTERVAL);
}