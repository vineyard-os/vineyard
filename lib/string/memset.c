#include <string.h>

void* memset(void* s, int c, size_t n) {
	volatile unsigned char *ptr = (volatile unsigned char *) s;

	while(n --> 0) {
		*ptr++ = (unsigned char) c;
	}

	return s;
}
