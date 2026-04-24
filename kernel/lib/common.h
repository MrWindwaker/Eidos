#ifndef COMMON_H
#define COMMON_H

#include <stdarg.h>
#include <stdint.h>

void printf(const char *fmt, ...);
void println(const char *fmt, ...);
void vprintf(const char *fmt, va_list vargs);
void panic(const char *fmt, ...);

#endif