#include <string.h>

int strcmp(const char* a, const char* b) {
	for(size_t i = 0; 1; i++) {
		unsigned char ac = (unsigned char) a[i];
		unsigned char bc = (unsigned char) b[i];

		if(ac == '\0' && bc == '\0') {
			return 0;
		}

		if(ac < bc) {
			return -1;
		}

		if(ac > bc) {
			return 1;
		}
	}
}
