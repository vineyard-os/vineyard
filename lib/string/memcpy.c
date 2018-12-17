#include <stddef.h>
#include <string.h>

/* TODO: this implementation is naive, improve this! */
void *memcpy(void * restrict s1, const void * restrict s2, size_t n) {
	char *dst = (char *) s1;
	const char *src = (char *) s2;

	while(n --> 0) {
		*dst++ = *src++;
	}

	return s1;
}
