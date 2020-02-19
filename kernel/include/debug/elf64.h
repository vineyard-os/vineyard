#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <vy.h>

#define EI_MAG0 0
#define EI_MAG0_VALUE 0x7F
#define EI_MAG1 1
#define EI_MAG1_VALUE 'E'
#define EI_MAG2 2
#define EI_MAG2_VALUE 'L'
#define EI_MAG3 3
#define EI_MAG3_VALUE 'F'
#define EI_CLASS 4
#define EI_DATA 5
#define EI_VERSION 6
#define EI_OSABI 7
#define EI_ABIVERSION 8
#define EI_PAD 9
#define EI_NIDENT 16

#define ELF_CLASS_32 1
#define ELF_CLASS_64 2

#define ELF_DATA_2LSB 1
#define ELF_DATA_2MSB 2

#define ELF_OSABI_SYSTEMV 0

#define EM_X86_64 62

#define ET_NONE 0
#define ET_REL 1
#define ET_EXEC 2
#define ET_DYN 3
#define ET_CORE 4

#define PT_NULL 0
#define PT_LOAD 1
#define PT_DYNAMIC 2
#define PT_INTERP 3
#define PT_NOTE 4
#define PT_SHLIB 5
#define PT_PHDR 6

#define PT_X 1
#define PT_W 2
#define PT_R 4

#define STT_NOTYPE  0
#define STT_OBJECT  1
#define STT_FUNC    2
#define STT_SECTION 3
#define STT_FILE    4

#define ELF64_ST_BIND(i) (((i) >> 4) & 0xF)
#define ELF64_ST_TYPE(i) ((i) & 0xF)
#define ELF64_ST_INFO(b, t) ((((b) << 4) & 0xF) | ((t) & 0xF))

typedef uint64_t elf64_addr_t;
typedef uint64_t elf64_off_t;
typedef uint16_t elf64_half_t;
typedef uint32_t elf64_word_t;
typedef int32_t elf64_sword_t;
typedef uint64_t elf64_xword_t;
typedef int64_t elf64_sxword_t;

typedef struct {
	unsigned char e_ident[EI_NIDENT];
	elf64_half_t e_type;
	elf64_half_t e_machine;
	elf64_word_t e_version;
	elf64_addr_t e_entry;
	elf64_off_t e_phoff;
	elf64_off_t e_shoff;
	elf64_word_t e_flags;
	elf64_half_t e_ehsize;
	elf64_half_t e_phentsize;
	elf64_half_t e_phnum;
	elf64_half_t e_shentsize;
	elf64_half_t e_shnum;
	elf64_half_t e_shstrndx;
} vy_packed elf64_ehdr_t;

typedef struct {
	elf64_word_t sh_name;
	elf64_word_t sh_type;
	elf64_xword_t sh_flags;
	elf64_addr_t sh_addr;
	elf64_off_t sh_offset;
	elf64_xword_t sh_size;
	elf64_word_t sh_link;
	elf64_word_t sh_info;
	elf64_xword_t sh_addralign;
	elf64_xword_t sh_entsize;
} vy_packed elf64_shdr_t;

typedef struct {
	elf64_word_t p_type;
	elf64_word_t p_flags;
	elf64_off_t p_offset;
	elf64_addr_t p_vaddr;
	elf64_addr_t p_paddr;
	elf64_xword_t p_filesz;
	elf64_xword_t p_memsz;
	elf64_xword_t p_align;
} vy_packed elf64_phdr_t;

typedef struct {
	elf64_word_t  st_name;
	unsigned char st_info;
	unsigned char st_other;
	elf64_half_t  st_shndx;
	elf64_addr_t  st_value;
	elf64_xword_t st_size;
} vy_packed elf64_sym_t;
