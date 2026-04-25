#ifndef CLINT_H
#define CLINT_H

#include <stdint.h>

#define CLINT_BASE 0x2000000UL
#define CLINT_MTIME (*(volatile uint64_t *)(CLINT_BASE + 0xbff8))
#define CLINT_MTIMECMP (*(volatile uint64_t *)(CLINT_BASE + 0x4000))

#define TIMER_INTERVAL 10000000

void clint_init(void);
void clint_rest_timer(void);

#endif