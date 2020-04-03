#include <string.h>

char *strrchr(const char *str, int c) {
	const unsigned char *s = (const unsigned char *) str;
	const char *last = NULL;

	for(size_t i = 0; 1; i++) {
		if(s[i] == (unsigned char) c) {
			last = str + i;
		}

		if(!s[i]) {
			break;
		}
	}

	return (char *) last;
}
