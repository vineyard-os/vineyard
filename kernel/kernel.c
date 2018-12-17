#include <efi.h>
#include <efi/protocol/file.h>

static efi_status efi_print(const char *str, efi_system_table *st) {
	for(const char *s = str; *s; s++) {
		uint16_t out[2] = { (uint16_t) *s, 0 };

		if(*s == '\n') {
			uint16_t nl[2] = { '\r', '\0' };
			efi_status status = st->ConOut->OutputString(st->ConOut, (char16_t *) nl);

			if(EFI_ERROR(status)) {
				return status;
			}
		}

		efi_status status = st->ConOut->OutputString(st->ConOut, (char16_t *) out);

		if(EFI_ERROR(status)) {
			return status;
		}
	}

	return EFI_SUCCESS;
}

efi_status main(efi_handle handle, efi_system_table *st, uintptr_t copy);

efi_status main(efi_handle handle __attribute__((unused)), efi_system_table *st, uintptr_t copy __attribute__((unused))) {
	efi_print("vineyard", st);

	for(;;);

	return EFI_SUCCESS;
}
