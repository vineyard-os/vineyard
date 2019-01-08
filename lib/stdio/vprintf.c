#include <driver/framebuffer.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

static size_t callback(void *ctx __attribute__((unused)), const char *string, size_t length) {
	size_t ret = length;

	while(*string && length > 0) {
		string += terminal_putc(string);
		length--;
	}

	return ret;
}

int vprintf(const char *restrict format, va_list list) {
	int ret = vcbprintf(NULL, callback, format, list);

	return ret;
}
