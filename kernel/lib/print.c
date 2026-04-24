#include "../drivers/uart/uart.h"
#include "common.h"

void vprintf(const char *fmt, va_list vargs)
{
    while (*fmt)
    {
        if (*fmt == '%')
        {
            fmt++;
            switch (*fmt)
            {
            case '\0':
                uart_putc('%');
                goto end;
            case '%':
                uart_putc('%');
                break;
            case 's':
            {
                const char *s = va_arg(vargs, const char *);
                while (*s)
                {
                    uart_putc(*s);
                    s++;
                }
                break;
            }
            case 'd':
            {
                int value = va_arg(vargs, int);
                unsigned magnitude = value;
                if (value < 0)
                {
                    uart_putc('-');
                    magnitude = -magnitude;
                }

                unsigned divisor = 1;
                while (magnitude / divisor > 9)
                    divisor *= 10;

                while (divisor > 0)
                {
                    uart_putc('0' + magnitude / divisor);
                    magnitude %= divisor;
                    divisor /= 10;
                }

                break;
            }
            case 'x':
            {
                uart_putc('0');
                uart_putc('x');
                uint64_t value = va_arg(vargs, uint64_t);
                for (int i = 15; i >= 0; i--)
                {
                    unsigned nibble = (value >> (i * 4)) & 0xf;
                    uart_putc("0123456789abcdef"[nibble]);
                }
                break;
            }
            default:
                uart_putc('%');
                uart_putc(*fmt);
            }
        }
        else
        {
            uart_putc(*fmt);
        }

        fmt++;
    }
end:;
}

void println(const char *fmt, ...)
{
    va_list vargs;
    va_start(vargs, fmt);
    vprintf(fmt, vargs);
    va_end(vargs);
    uart_putc('\n');
}

void printf(const char *fmt, ...)
{
    va_list vargs;
    va_start(vargs, fmt);
    vprintf(fmt, vargs);
    va_end(vargs);
}