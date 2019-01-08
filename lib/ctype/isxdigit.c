#include <ctype.h>

int isxdigit(int c) {
	if(isdigit(c)) {
		return 1;
	}

	if('a' <= c && c <= 'f') {
		return 1;
	}

	if('A' <= c && c <= 'F') {
		return 1;
	}

	return 0;
}
