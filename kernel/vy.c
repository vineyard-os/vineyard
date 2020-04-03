#include <stdio.h>
#include <stdlib.h>
#include <vy.h>

uint64_t min(uint64_t a, uint64_t b) {
	if(a > b) {
		return b;
	}

	return a;
}

void cleanup_vy_free(void *ptr) {
	free(*(void**) ptr);
}
