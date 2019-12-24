#include <stdio.h>

int asprintf(char **strp, const char *format, ...) {
	va_list list;
	va_start(list, format);

	int ret = vasprintf(strp, format, list);

	va_end(list);
	return ret;
}