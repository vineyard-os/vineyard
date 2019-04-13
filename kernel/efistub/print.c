#include <efistub/uefi.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

#define PRINT(x) st->ConOut->OutputString(st->ConOut, (x))

efi_status efi_print(const char *str) {
	return efi_printn(str, strlen(str));
}

efi_status efi_printn(const char *str, size_t n) {
	size_t i = 0;

	for(const char *s = str; *s && i < n; s++, i++) {
		uint16_t out[2] = { (uint16_t) *s, 0 };

		if(*s == '\n') {
			uint16_t nl[2] = { '\r', '\0' };
			efi_status status = PRINT((char16_t *) nl);

			if(EFI_ERROR(status)) {
				return status;
			}
		}

		efi_status status = PRINT((char16_t *) out);

		if(EFI_ERROR(status)) {
			return status;
		}
	}

	return EFI_SUCCESS;
}

static size_t callback(void *ctx __attribute__((unused)), const char *string, size_t length) {
	efi_printn(string, length);

	return length;
}

int efi_printf(const char *restrict format, ...) {
	va_list list;
	va_start(list, format);

	int ret = vcbprintf(NULL, callback, format, list);

	va_end(list);

	return ret;
}
