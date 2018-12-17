#include <boot/info.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <uefi.h>

efi_status efi_get_image(efi_loaded_image_protocol **prot) {
	efi_status status;
	efi_guid guid = EFI_LOADED_IMAGE_PROTOCOL_GUID;

	status = info.st->BootServices->HandleProtocol(info.handle, &guid, (void **) prot);
	EFIERR(status);

	return EFI_SUCCESS;
}
