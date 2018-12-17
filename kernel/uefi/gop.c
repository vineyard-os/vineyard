#include <boot/info.h>
#include <string.h>
#include <stdio.h>
#include <uefi.h>

#define ERR(x) if(EFI_ERROR((x))) return (x)

efi_status efi_gop_get(efi_graphics_output_protocol **out) {
	efi_guid gop_guid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;

	uint64_t num;
	efi_handle *handles;
	efi_status status;

	status = info.st->BootServices->LocateHandleBuffer(ByProtocol, &gop_guid, NULL, &num, &handles);
	ERR(status);

	for(uint64_t i = 0; i < num; ++i) {
		efi_graphics_output_protocol *gop;
		status = info.st->BootServices->OpenProtocol(handles[i], &gop_guid, (void **) &gop, info.handle, NULL, EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);
		ERR(status);

		if(gop) {
			*out = gop;
			return EFI_SUCCESS;
		}
	}

	out = NULL;

	return EFI_UNSUPPORTED;
}
