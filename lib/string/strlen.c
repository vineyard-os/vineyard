#include <string.h>

size_t strlen(const char *str) {
	size_t ret = 0;

	for(; *str; ret++, str++);

	return ret;
}
