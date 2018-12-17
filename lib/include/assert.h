#pragma once

void __assert_fail(const char *assertion, const char *file, unsigned int line, const char *function);

#if !defined(NDEBUG)
	#define assert(expr) (expr) ? (void)0 : __assert_fail(#expr, __FILE__, __LINE__, __FUNCTION__)
#else
	#define assert(expr) (void)0
#endif

#define static_assert(expr, msg) _Static_assert(expr, msg)
