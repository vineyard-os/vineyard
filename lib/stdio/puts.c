#include <driver/framebuffer.h>
#include <driver/uart.h>
#include <stdio.h>

int puts(const char *str) {
	int ret = 0;

	while(*str) {
		str += terminal_putc(str);

		ret++;
	}

	terminal_putc("\n");

	return ret;
}
