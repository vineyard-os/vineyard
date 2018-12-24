#pragma once

#include <stdint.h>
#include <sys/types.h>

#define PROT_NONE 0x0
#define PROT_READ 0x1
#define PROT_WRITE 0x2
#define PROT_EXEC 0x4

#define MAP_ANONYMOUS 0x1
#define MAP_PRIVATE 0x2

#define MAP_FAILED ((void *)-1)

void *mmap(void *addr, size_t len, int prot, int flags, int flides, off_t off);
int munmap(void *addr, size_t len);
