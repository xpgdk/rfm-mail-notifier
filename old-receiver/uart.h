#ifndef UART_H
#define UART_H 1

#include <msp430.h>
#include <stdbool.h>
#include <stdint.h>

#define TXD BIT1 // TXD on P1.1
#define RXD BIT2 // RXD on P1.2

void uart_init(void);
bool uart_getc(uint8_t *c);
void uart_putc(uint8_t c);
void uart_puts(const char *s);
void uart_recv_int(void);

#endif

