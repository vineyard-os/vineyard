#include <assert.h>
#include <boot/info.h>
#include <boot/panic.h>
#include <cpu/cr.h>
#include <mm/page.h>
#include <mm/physical.h>
#include <mm/tlb.h>
#include <mm/virtual.h>
#include <stdio.h>
#include <string.h>
#include <uefi.h>

#define STACK_SIZE 509

/* packed (4096-byte = page-sized) stack for fast single-page physical allocation */
struct mm_physical_stack {
	uintptr_t prev;
	/* physical address of the next stack */
	uintptr_t next;
	/* number of pages on this stack */
	uintptr_t used;
	/* list of free pages */
	uintptr_t data[STACK_SIZE];
} __attribute__((packed));

/* make sure that the stack struct is actually the size it is supposed to be (4096 bytes) */
static_assert(sizeof(struct mm_physical_stack) == 0x1000, "unexpected struct mm_physical_stack size");

/* strings for printing out EFI memory types */
static const char *memory_map_types[] = {
	[EfiReservedMemoryType] = "Reserved",
	[EfiLoaderCode] = "Loader Code",
	[EfiLoaderData] = "Loader Data",
	[EfiBootServicesCode] = "Boot Services Code",
	[EfiBootServicesData] = "Boot Services Data",
	[EfiRuntimeServicesCode] = "Runtime Services Code",
	[EfiRuntimeServicesData] = "Runtime Services Data",
	[EfiConventionalMemory] = "Conventional Memory",
	[EfiUnusableMemory] = "Unusable Memory",
	[EfiACPIReclaimMemory] = "ACPI Reclaimable Memory",
	[EfiACPIMemoryNVS] = "ACPI NVS",
	[EfiMemoryMappedIO] = "Memory Mapped I/O",
	[EfiMemoryMappedIOPortSpace] = "Memory Mapped I/O Port Space",
	[EfiPalCode] = "Pal Code",
	[EfiPersistentMemory] = "Persistent Memory",
};

/* pointer to the level 1 page table holding the stack at index 511 */
uint64_t *mm_physical_stack_pml1;
/* page tables for mapping in the stack's address */
static uint64_t stack_pml3[512] __attribute__((__aligned__(0x1000)));
static uint64_t stack_pml2[512] __attribute__((__aligned__(0x1000)));
static uint64_t stack_pml1[512] __attribute__((__aligned__(0x1000)));

/* static allocation of the first stack */
static uint64_t root_stack[512] __attribute__((__aligned__(0x1000)));
/* physical address of the first stack */
static uint64_t root_stack_phys = (uintptr_t) &root_stack - 0xFFFFFFFF80000000 + 0x100000;
/* virtual address of the currently mapped stack */
static struct mm_physical_stack *stack = (struct mm_physical_stack *) STACK_ADDR;

static void mm_physical_next_stack(uintptr_t addr) {
	mm_physical_stack_pml1[511] = addr | PAGE_PRESENT | PAGE_WRITE;
	mm_tlb_invlpg(STACK_ADDR);
}

static void mm_physical_init_stack(void) {
	{
		uintptr_t pml4i = (STACK_ADDR >> 39) & 0x1FF;
		uintptr_t pml3i = (STACK_ADDR >> 30) & 0x1FF;
		uintptr_t pml2i = (STACK_ADDR >> 21) & 0x1FF;
		uint64_t *table = (uint64_t *) (uint64_t) cr3_read();

		/* TODO: maybe check whether page is present? investigate the correct way */
		if(!(table[pml4i] & PAGE_PRESENT)) {
			table[pml4i] = (((uint64_t) stack_pml3) - 0xFFFFFFFF80000000 + 0x100000) | PAGE_PRESENT | PAGE_WRITE;
		}

		table = (uint64_t *) (table[pml4i] & ~0xFFFUL);

		if(!(table[pml3i] & PAGE_PRESENT)) {
			table[pml3i] = (((uint64_t) stack_pml2) - 0xFFFFFFFF80000000 + 0x100000) | PAGE_PRESENT | PAGE_WRITE;
		}

		table = (uint64_t *) (table[pml3i] & ~0xFFFUL);

		if(!(table[pml2i] & PAGE_PRESENT)) {
			table[pml2i] = (((uint64_t) stack_pml1) - 0xFFFFFFFF80000000 + 0x100000) | PAGE_PRESENT | PAGE_WRITE;
		}

		mm_physical_stack_pml1 = (uint64_t *) (table[pml2i] & ~0xFFFUL);
	}

	mm_physical_next_stack(root_stack_phys);
	memset(root_stack, 0, sizeof(*root_stack));
}

static uint64_t mm_physical_create_stack(uintptr_t phys) {
	struct mm_physical_stack *new = (struct mm_physical_stack *) STACK_ADDR_TEMP;
	uintptr_t current = mm_physical_stack_pml1[511] & 0x000FFFFFFFFFF000;

	mm_physical_stack_pml1[510] = phys | PAGE_PRESENT | PAGE_WRITE;

	memset(new, 0, 24);
	memcpy(new, &current, 8);

	mm_physical_stack_pml1[510] = 0;
	mm_tlb_invlpg(STACK_ADDR_TEMP);

	return phys;
}

uint64_t mm_physical_get(void) {
	if(stack->used > 0) {
		uintptr_t ret = stack->data[stack->used - 1];
		stack->data[stack->used - 1] = 0;
		stack->used--;

		return ret;
	} else {
		if(stack->prev) {
			mm_physical_next_stack(stack->prev);
		} else {
			panic("out of physical memory");
		}

		return mm_physical_get();
	}
}

void mm_physical_mark_free(uint64_t addr) {
	if(stack && stack->used < STACK_SIZE) {
		stack->data[stack->used++] = addr;
	} else {
		stack->next = mm_physical_create_stack(mm_physical_get());

		mm_physical_next_stack(stack->next);
		stack->data[stack->used++] = addr;
	}
}

void mm_physical_init(void) {
	size_t up = 0;

	mm_physical_init_stack();

	for(size_t _mmap_i = 0; _mmap_i < info.efi_memory_map_entries; _mmap_i++) {
		efi_memory_descriptor *desc = (void *) ((uint8_t *) info.efi_memory_map + (info.efi_memory_map_descriptor_size * _mmap_i));
		uintptr_t start = (uintptr_t) desc->PhysicalStart;
		size_t size = desc->NumberOfPages << PAGE_SHIFT;
		size_t pages = desc->NumberOfPages;
		uintptr_t end = (uintptr_t) desc->PhysicalStart + (desc->NumberOfPages << PAGE_SHIFT) - 1;
		efi_memory_type type = (efi_memory_type) desc->Type;

		if(type != EfiMemoryMappedIO && type != EfiMemoryMappedIOPortSpace && type != EfiUnusableMemory) {
			up += size;
		}

		printf("[pmm]	normal: %#018lx - %#018lx (%zu KiB) [%s]\n", start, end, size >> 10, memory_map_types[type]);

		if(type == EfiConventionalMemory || type == EfiBootServicesCode || type == EfiBootServicesData) {
			for(size_t i = 0; i < pages; i++) {
				mm_physical_mark_free(start + (i << PAGE_SHIFT));
			}
		}
	}
}
