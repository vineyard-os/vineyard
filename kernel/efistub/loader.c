#include <efi.h>
#include <efi/protocol/file.h>
#include <efistub/cpu.h>
#include <efistub/elf.h>
#include <efistub/elf64.h>
#include <efistub/fs.h>
#include <efistub/loader.h>
#include <efistub/virt.h>
#include <efistub/uefi.h>
#include <stdbool.h>
#include <string.h>

efi_handle handle;
efi_system_table *st;

efi_status efi_main(efi_handle image_handle, efi_system_table *systab) {
	efi_status status;

	st = systab;
	handle = image_handle;

	/* OVMF (and possibly other UEFI implementations) set the WP bit in cr0, which prevents us from modifying page tables */
	efi_virt_wp_disable();

	efi_msr_write(0xC0000080, efi_msr_read(0xC0000080) | 0x800);

	status = st->BootServices->SetWatchdogTimer(0, 0, 0, NULL);
	ERR(status);

	status = st->ConOut->ClearScreen(st->ConOut);
	ERR(status);

	efi_file_protocol *kernel;
	status = efi_fs_load_kernel(&kernel);
	ERR(status);

	/* map a complete copy of the binary into memory (used for parsing by the kernel itself) */
	void *copy = 0;
	efi_file_info *info = NULL;
	efi_guid guid = EFI_FILE_INFO_GUID;
	uintptr_t info_len = 0x1;
	status = kernel->GetInfo(kernel, &guid, &info_len, info);

	if(status != EFI_SUCCESS && status != EFI_BUFFER_TOO_SMALL) {
		efi_panic(L"couldn't get info", status);
	}

	status = st->BootServices->AllocatePool(EfiLoaderData, info_len, (void **) &info);
	ERR(status);

	status = kernel->GetInfo(kernel, &guid, &info_len, info);

	status = st->BootServices->AllocatePool(EfiLoaderData, info->FileSize, &copy);

	if(EFI_ERROR(status)) {
		efi_panic(L"unable to allocate space for kernel", status);
	}

	status = kernel->Read(kernel, &info->FileSize, copy);
	ERR(status);

	status = kernel->SetPosition(kernel, 0);
	ERR(status);

	elf64_ehdr_t header;
	status = efi_fs_read_header(kernel, &header);
	ERR(status);

	/* TODO: this only checks some params choosen randomly; properly check everything we should! */
	if(header.e_ident[EI_MAG0] == EI_MAG0_VALUE && header.e_ident[EI_MAG1] == EI_MAG1_VALUE && header.e_ident[EI_MAG2] == EI_MAG2_VALUE && header.e_ident[EI_MAG3] == EI_MAG3_VALUE) {
		if(header.e_ident[EI_CLASS] != ELFCLASS64 || header.e_ident[EI_DATA] != ELFDATA2LSB || header.e_type != ET_EXEC || header.e_machine != EM_X86_64) {
			efi_panic(L"invalid ELF header info", EFI_INVALID_PARAMETER);
		}
	} else {
		efi_panic(L"invalid ELF magic", EFI_INVALID_PARAMETER);
	}

	elf64_phdr_t *phdrs;
	efi_fs_read_phdrs(kernel, header, &phdrs);

	for(elf64_phdr_t *phdr = phdrs; (char *) phdr < (char *) phdrs + header.e_phnum * header.e_phentsize; phdr = (elf64_phdr_t *) ((char *) phdr + header.e_phentsize)) {
		if(phdr->p_type == PT_LOAD) {
			size_t pages = (phdr->p_memsz + 0xfff) / 0x1000;
			uintptr_t physical = phdr->p_paddr;
			status = st->BootServices->AllocatePages(AllocateAddress, EfiLoaderData, pages, &physical);

			if(EFI_ERROR(status)) {
				efi_panic(L"error mapping kernel", status);
			}

			kernel->SetPosition(kernel, phdr->p_offset);
			uint64_t size = phdr->p_filesz;
			kernel->Read(kernel, &size, (void *) physical);
			uintptr_t flags = efi_elf_phdr_flags(phdr->p_flags);

			efi_virt_map(physical, phdr->p_vaddr, pages, flags);

			if(phdr->p_memsz - phdr->p_filesz > 0) {
				memset((uint8_t *) phdr->p_paddr + phdr->p_filesz, 0, phdr->p_memsz - phdr->p_filesz);
			}
		}
	}

	status = st->BootServices->FreePool(phdrs);

	if(EFI_ERROR(status)) {
		efi_panic(L"unable to free program headers", status);
	}

	status = kernel->Close(kernel);

	if(EFI_ERROR(status)) {
		efi_panic(L"error closing kernel file", status);
	}

	uintptr_t stack;
	status = st->BootServices->AllocatePages(AllocateAnyPages, EfiLoaderData, 33, &stack);

	if(EFI_ERROR(status)) {
		efi_panic(L"unable to allocate page", status);
	}

	efi_virt_map(stack, STACK, 33, PAGE_PRESENT | PAGE_WRITE | PAGE_NX);
	efi_loader_jump_to_kernel(handle, st, (uintptr_t) copy, header.e_entry);

	for(;;);

	return EFI_SUCCESS;
}
