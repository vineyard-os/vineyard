#include <debug/elf64.h>
#include <debug/trace.h>
#include <mm/virtual.h>
#include <stdio.h>
#include <string.h>

static elf64_sym_t *symtab;
static size_t symtab_size;

static uintptr_t strtab;
static size_t strtab_size;

void debug_init(uintptr_t copy, size_t size) {
	size = PAGE_UP(size);
	uintptr_t virt = mm_virtual_alloc(size >> 12);

	mm_virtual_map(copy & ~0xFFFU, virt, size >> 12, PAGE_PRESENT);

	elf64_ehdr_t *header = (void *) (virt + (copy & 0xFFFU));
	elf64_shdr_t *section = (void *) ((uintptr_t) header + header->e_shoff);
	uintptr_t section_str_tab = (uintptr_t) header + section[header->e_shstrndx].sh_offset;

	for(size_t i = 0; i < header->e_shnum; i++, section++) {
		if(!section->sh_type) {
			continue;
		}

		const char *name = (const char *) (section_str_tab + section->sh_name);

		if(!strncmp(".symtab", name, 7)) {
			symtab = (void *) ((uintptr_t) header + section->sh_offset);
			symtab_size = section->sh_size;
		} else if(!strncmp(".strtab", name, 7)) {
			strtab = (uintptr_t) header + section->sh_offset;
			strtab_size = section->sh_size;
		}
	}
}

vy_unused static const char *debug_trace_get_symbol(uintptr_t addr) {
	if(strtab && symtab) {
		elf64_sym_t *sym = symtab;

		for(size_t i = 0; i < (symtab_size / sizeof(*symtab)); i++, sym++) {
			if(ELF64_ST_TYPE(sym->st_info) != STT_FUNC) {
				continue;
			}

			uint64_t offset = (uint64_t) sym->st_value;

			if(addr >= offset && addr < (offset + sym->st_size)) {
				return (const char *) (strtab + sym->st_name);
			}
		}
	}

	return "<unknown>";
}

extern uintptr_t main;

__attribute__((no_sanitize("undefined"))) void debug_trace(void) {
#ifndef __OPTIMIZE__
	uint64_t original = debug_trace_rbp();
	uint64_t *rbp = (uint64_t *) original;

	while(rbp) {
		uintptr_t rip = rbp[1];

		const char *name = debug_trace_get_symbol(rip);

		printf("  [%0#18lx] %s\n", rip, name);

		if((rip & KERNEL_START) != KERNEL_START) {
			break;
		}

		if(!strncmp("main", name, 4) || !strncmp("isr_dispatch", name, 12)) {
			break;
		}

		rbp = (uint64_t *) rbp[0];
	}
#endif
}
