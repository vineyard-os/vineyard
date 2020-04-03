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

static uint64_t stack_pml3[512] __attribute__((__aligned__(0x1000)));
static uint64_t stack_pml2[512] __attribute__((__aligned__(0x1000)));
static uint64_t stack_pml1[512] __attribute__((__aligned__(0x1000)));

/* static allocation of the first stack */
static uint64_t root_stack[512] __attribute__((__aligned__(0x1000)));
/* physical address of the first stack */
static uint64_t root_stack_phys;
/* virtual address of the currently mapped stack */
static struct mm_physical_stack *stack = (struct mm_physical_stack *) STACK_ADDR;

static void mm_physical_next_stack(uintptr_t addr) {
	mm_physical_stack_pml1[511] = addr | PAGE_PRESENT | PAGE_WRITE;
	mm_tlb_invlpg(STACK_ADDR);
}

static void mm_physical_init_stack(void) {
	struct vm_indices i;
	mm_virtual_indices(&i, STACK_ADDR);

	if(!(i.pml4[i.pml4i] & PAGE_PRESENT)) {
		i.pml4[i.pml4i] = mm_virtual_get_phys((uintptr_t) &stack_pml3) | PAGE_PRESENT | PAGE_WRITE;

		memset(i.pml3, 0, 0x1000);
	}

	if(!(i.pml3[i.pml3i] & PAGE_PRESENT)) {
		i.pml3[i.pml3i] = mm_virtual_get_phys((uintptr_t) &stack_pml2) | PAGE_PRESENT | PAGE_WRITE;

		memset(i.pml2, 0, 0x1000);
	}

	if(!(i.pml2[i.pml2i] & PAGE_PRESENT)) {
		i.pml2[i.pml2i] = mm_virtual_get_phys((uintptr_t) &stack_pml1) | PAGE_PRESENT | PAGE_WRITE;

		memset(i.pml1, 0, 0x1000);
	}

	mm_physical_stack_pml1 = (uint64_t *) i.pml1;

	root_stack_phys = mm_virtual_get_phys((uintptr_t) &root_stack);
	mm_virtual_map(root_stack_phys, STACK_ADDR, 1, PAGE_PRESENT | PAGE_WRITE);

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

static vy_unused const char *get_memory_type(efi_memory_type type) {
	if(type <= EfiPersistentMemory) {
		return memory_map_types[type];
	} else {
		return "invalid type";
	}
}

void mm_physical_init(void) {
	mm_physical_init_stack();

	for(size_t i = 0; i < info.efi_memory_map_entries; i++) {
		efi_memory_descriptor *desc = (void *) ((uint8_t *) info.efi_memory_map + (info.efi_memory_map_descriptor_size * i));

#ifdef CONFIG_PMM_DEBUG
		if(!desc->NumberOfPages) {
			printf("[efi]	entry %zu invalid size\n", i);
		} else {
			printf("[efi]	normal: %#018lx - %#018lx (%zu pages) [%s]\n", desc->PhysicalStart, desc->PhysicalStart + (desc->NumberOfPages << 12), desc->NumberOfPages, get_memory_type(desc->Type));
		}
#endif

		if(desc->Type == EfiConventionalMemory || desc->Type == EfiBootServicesCode || desc->Type == EfiBootServicesData) {
			for(size_t j = 0; j < desc->NumberOfPages; j++) {
				mm_physical_mark_free(desc->PhysicalStart + (j << PAGE_SHIFT));
			}
		}
	}
}
