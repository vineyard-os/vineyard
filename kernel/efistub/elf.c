#include <efistub/elf.h>
#include <efistub/elf64.h>
#include <efistub/virt.h>

uintptr_t efi_elf_phdr_flags(uintptr_t f) {
	uintptr_t flags = PAGE_PRESENT;

	if(!(f | PT_X)) {
		flags |= PAGE_NX;
	}

	if(f | PT_W) {
		flags |= PAGE_WRITE;
	}

	return flags;
}
