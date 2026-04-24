#include <stdarg.h>
#include "common.h"

void panic(const char *fmt, ...)
{
    va_list vargs;
    va_start(vargs, fmt);

    println("\n--- KERNEL PANIC ---");

    printf(fmt);
    va_end(vargs);

    println("\n--------------------");

    for (;;)
        asm volatile("wfi");
}