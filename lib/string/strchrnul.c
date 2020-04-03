#include <string.h>

char *strchrnul(const char *str, int c) {
	const unsigned char *s = (const unsigned char *) str;

	for(size_t i = 0; 1; i++) {
		if(s[i] == (unsigned char) c || !s[i]) {
			return (char *) str + i;
		}
	}
}
