#pragma once

#include <stddef.h>
#include <stdint.h>

void *memcpy(void * restrict s1, const void * restrict s2, size_t n) __attribute__((nonnull(1, 2)));
int memcmp(const void *str1, const void *str2, size_t count) __attribute__((nonnull(1, 2)));
void *memset(void *s, int c, size_t n) __attribute__((nonnull(1)));
size_t strlen(const char *str) __attribute__((nonnull(1)));
int strncmp(const char *s1, const char *s2, size_t n) __attribute__((nonnull(1, 2)));
char *strcpy(char * restrict dest, const char * restrict src) __attribute__((nonnull(1, 2)));
char *strcat(char *dest, const char *src) __attribute__((nonnull(1, 2)));
char *strncpy(char *dest, const char *src, size_t n) __attribute__((nonnull(1, 2)));
int strcmp(const char* a, const char* b) __attribute__((nonnull(1, 2)));
char *strncat(char *dest, const char *src, size_t n) __attribute__((nonnull(1, 2)));
char *strdup(const char *str) __attribute__((nonnull(1)));
char *strchr(const char *str, int c);
char *strrchr(const char *str, int c);
char *strchrnul(const char *str, int c);
