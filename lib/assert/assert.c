#include <assert.h>
#include <stdio.h>

_Noreturn void __assert_fail(const char *assertion, const char *file, unsigned int line, const char *function) {
	if(function) {
		printf("%s:%u: %s: %s\n", file, line, function, assertion);
	} else {
		printf("%s:%u: %s\n", file, line, assertion);
	}

	for(;;);
}
