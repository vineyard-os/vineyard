#include <stdlib.h>
#include <string.h>

char *strdup(const char *str) {
	size_t len = strlen(str);
	char *ret = malloc(len + 1);

	if(!ret) {
		return NULL;
	}

	memcpy(ret, str, len + 1);

	return ret;
}
