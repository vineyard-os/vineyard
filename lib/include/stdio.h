#pragma once

#ifndef __size_t_defined
#define __size_t_defined
#define __need_size_t
#include <stddef.h>
#endif

#include <stdarg.h>

int puts(const char *str);
__attribute__((format(printf, 1, 2))) int printf(const char *restrict format, ...);
__attribute__((format(printf, 1, 0))) int vprintf(const char *restrict format, va_list list);
__attribute__((format(printf, 3, 0))) int vcbprintf(void *ctx, size_t (*callback)(void *ctx, const char *string, size_t length), const char *format, va_list parameters);
