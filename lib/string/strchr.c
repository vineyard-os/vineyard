#include <string.h>

char *strchr(const char *str, int c) {
	char *ret = strchrnul(str, c);

	return (unsigned char) c == ((unsigned char *) ret)[0] ? ret : NULL;
}
