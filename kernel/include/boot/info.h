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
	efi_graphics_output_mode *gop_mode;

	uintptr_t copy;
	size_t copy_size;
} info_t;

extern info_t info;
