#include "uart.h"

#define UART_BASE 0x10000000UL

#define UART_THR (*(volatile unsigned char *)(UART_BASE + 0))
#define UART_LSR (*(volatile unsigned char *)(UART_BASE + 5))
#define UART_LSR_THRE (1 << 5)

#define UART_LCR (*(volatile unsigned char *)(UART_BASE + 3))
#define UART_DLL (*(volatile unsigned char *)(UART_BASE + 0))
#define UART_DLM (*(volatile unsigned char *)(UART_BASE + 1))
#define UART_FCR (*(volatile unsigned char *)(UART_BASE + 2))
#define UART_IER (*(volatile unsigned char *)(UART_BASE + 1))

void uart_init(void)
{
    UART_IER = 0x00;
    UART_LCR = 0x80;

    UART_DLL = 0x01;
    UART_DLM = 0x00;

    UART_LCR = 0x03;
    UART_FCR = 0x07;
}

void uart_putc(char c)
{
    while ((UART_LSR & UART_LSR_THRE) == 0)
        ;
    UART_THR = c;
}

void uart_puts(const char *s)
{
    while (*s)
    {
        uart_putc(*s++);
    }
}