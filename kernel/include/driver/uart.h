#pragma once

#include <stdint.h>

void uart_init(void);
void uart_putc(uint8_t c);
__attribute__((format(printf, 1, 2))) int uart_printf(const char *restrict format, ...);
