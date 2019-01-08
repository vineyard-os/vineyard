#pragma once

#include <efi.h>

typedef struct {
	efi_handle handle;
	efi_system_table *st;
	efi_loaded_image_protocol *image;
	efi_memory_descriptor *efi_memory_map;
	size_t efi_memory_map_entries;
	uint64_t efi_memory_map_key;
	size_t efi_memory_map_descriptor_size;
	void *rsdp;
} info_t;

extern info_t info;
