#pragma once

#include <stdint.h>
#include <kconfig.h>

#define vy_unused __attribute__((unused))
#define vy_pure __attribute__((pure))
#define vy_packed __attribute__((packed))

typedef uintptr_t vy_status_t;

#define VY_OK 0UL
#define VY_ERR 1UL

#define ALIGN_UP(addr, alignment)	((addr + (alignment - 1U)) & ~(alignment - 1U))

#define HELPER0(x) #x

#ifdef __clang__
#define HELPER1(x) HELPER0(clang diagnostic ignored x)
#define no_warn_clang(x) _Pragma(HELPER1(#x))
#define no_warn_gcc(x)
#define warn_push _Pragma("clang diagnostic push")
#define warn_pop _Pragma("clang diagnostic pop")
#elif defined(__GNUC__)
#define HELPER1(x) HELPER0(GCC diagnostic ignored x)
#define no_warn_clang(x)
#define no_warn_gcc(x) _Pragma(HELPER1(#x))
#define warn_push _Pragma("GCC diagnostic push")
#define warn_pop _Pragma("GCC diagnostic pop")
#else
#error unknown compiler
#endif

#define no_warn(x) _Pragma(HELPER1(#x))

uint64_t min(uint64_t a, uint64_t b);

void cleanup_vy_free(void *ptr);

#define vy_free __attribute__((cleanup(cleanup_vy_free)))
