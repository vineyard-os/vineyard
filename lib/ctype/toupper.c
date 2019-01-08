#include <ctype.h>

int toupper(int c) {
	if('a' <= c && c <= 'z') {
		return 'A' + c - 'a';
	}

	return c;
}
