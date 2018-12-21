#include <efistub/fs.h>
#include <efistub/uefi.h>

efi_status efi_fs_load_kernel(efi_file_protocol **kernel) {
	efi_status status;

	efi_guid loaded_image_guid = EFI_LOADED_IMAGE_PROTOCOL_GUID;
	efi_guid simple_fs_guid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;

	efi_loaded_image_protocol *loaded_image;
	status = st->BootServices->HandleProtocol(handle, &loaded_image_guid, (void **) &loaded_image);
	EFIERR(status);

	efi_simple_file_system_protocol *file_system;
	status = st->BootServices->HandleProtocol(loaded_image->DeviceHandle, &simple_fs_guid, (void **) &file_system);
	EFIERR(status);

	efi_file_protocol *root;
	status = file_system->OpenVolume(file_system, &root);
	EFIERR(status);

	efi_status s = root->Open(root, kernel, (char16_t *) L"kernel", EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);
	if(s != EFI_SUCCESS) {
		efi_panic(L"kernel image not found\n", s);
	}

	return EFI_SUCCESS;
}

efi_status efi_fs_read_header(efi_file_protocol *kernel, elf64_ehdr_t *header) {
	efi_status status;

	uint64_t size = sizeof(*header);
	status = kernel->Read(kernel, &size, header);
	EFIERR(status);

	return EFI_SUCCESS;
}

efi_status efi_fs_read_phdrs(efi_file_protocol *kernel, elf64_ehdr_t header, elf64_phdr_t **phdrs) {
	efi_status status = kernel->SetPosition(kernel, header.e_phoff);
	EFIERR(status);
	uint64_t size = (size_t) header.e_phnum * header.e_phentsize;
	status = st->BootServices->AllocatePool(EfiLoaderData, size, (void **) phdrs);
	EFIERR(status);
	status = kernel->Read(kernel, &size, *phdrs);
	EFIERR(status);

	return EFI_SUCCESS;
}
