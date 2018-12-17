#pragma once

#include <stddef.h>
#include <stdint.h>

void *memcpy(void * restrict s1, const void * restrict s2, size_t n);
int memcmp(const void *str1, const void *str2, size_t count);
void *memset(void *s, int c, size_t n);
size_t strlen(const char *str);
int strncmp(const char *s1, const char *s2, size_t n);
