#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct vasprintf_state {
	char *buf;
	size_t len;
	size_t used;
};

static size_t vasprintf_callback(void *ctx, const char *string, size_t len) {
	struct vasprintf_state *state = (struct vasprintf_state *) ctx;
	size_t len_needed = state->used + len + 1;

	if(state->len < len_needed) {
		size_t len_new = state->len >> 1;

		if(len_needed > len_new) {
			len_new = len_needed;
		}

		state->buf = (char *) realloc(state->buf, len_new);
		state->len = len_new;
	}

	memcpy(state->buf + state->used, string, len);
	state->used += len;

	return len;
}

int vasprintf(char **strp, const char *format, va_list ap) {
	struct vasprintf_state state;
	state.used = 0;
	state.len = 1024;
	state.buf = malloc(state.len);

	int ret = vcbprintf(&state, vasprintf_callback, format, ap);

	state.buf[state.used] = '\0';
	*strp = state.buf;

	return ret;
}