#include <string.h>

char *strncpy(char *dest, const char *src, size_t n) {
	size_t i;

	for(i = 0; i < n && src[i]; i++) {
		dest[i] = src[i];
	}

	for(; i < n; i++) {
		dest[i] = '\0';
	}

	return dest;
}
