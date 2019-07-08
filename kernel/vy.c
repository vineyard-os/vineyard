#include <stdio.h>
#include <stdlib.h>
#include <vy.h>

void cleanup_vy_free(void *ptr) {
	free(*(void**) ptr);
}