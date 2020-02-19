#include <boot/panic.h>
#include <debug/trace.h>
#include <driver/framebuffer.h>
#include <stdio.h>
#include <stdarg.h>

static size_t callback(void *ctx __attribute__((unused)), const char *string, size_t length) {
	size_t ret = length;

	while(*string && length > 0) {
		string += terminal_putc(string);
		length--;
	}

	return ret;
}

void panic(const char * restrict format, ...) {
	__asm volatile ("cli");

	va_list args;
	va_start(args, format);

	vcbprintf(NULL, callback, format, args);
	puts("");

	va_end(args);

	debug_trace();

	__asm volatile ("hlt");
	for(;;);

	__builtin_unreachable();
}
