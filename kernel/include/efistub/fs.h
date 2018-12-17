#pragma once

#include <efi.h>
#include <efistub/elf64.h>

efi_status efi_fs_load_kernel(efi_file_protocol **kernel);
efi_status efi_fs_read_header(efi_file_protocol *kernel, elf64_ehdr_t *header);
efi_status efi_fs_read_phdrs(efi_file_protocol *kernel, elf64_ehdr_t header, elf64_phdr_t **phdrs);
