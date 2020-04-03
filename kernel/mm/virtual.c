#include <assert.h>
#include <boot/panic.h>
#include <mm/page.h>
#include <mm/slab.h>
#include <mm/physical.h>
#include <mm/tlb.h>
#include <mm/virtual.h>
#include <mm/page_tables.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <vy.h>

#define VM_FOREACH(x) for(struct vm_node *x = list.head; x; x = x->next)

struct vm_node {
	struct vm_node *next;
	struct vm_node *prev;
	uintptr_t start;
	size_t len;
	uintptr_t flags;
};

struct vm_info {
	struct vm_node *head;
	struct vm_node *tail;
	size_t nodes;
};

static struct vm_info list = { NULL, NULL, 0 };
static slab_cache_t *cache = NULL;

vy_status_t mm_virtual_indices(struct vm_indices *i, uintptr_t addr) {
	if(addr >= HIGHER_HALF) {
		addr -= HIGHER_HALF;
		i->pml4i = 256 + ((addr >> PML4_SHIFT) & 0x1FF);
	} else {
		i->pml4i = (addr >> PML4_SHIFT) & 0x1FF;
	}

	i->pml3i = (addr >> PML3_SHIFT) & 0x1FF;
	i->pml2i = (addr >> PML2_SHIFT) & 0x1FF;
	i->pml1i = (addr >> PML1_SHIFT) & 0x1FF;

	i->pml4 = (uint64_t *) PML4_OFFSET;
	i->pml3 = (uint64_t *) (PML3_OFFSET + (i->pml4i << PML1_SHIFT));
	i->pml2 = (uint64_t *) (PML2_OFFSET + (i->pml4i << PML2_SHIFT) + (i->pml3i << PML1_SHIFT));
	i->pml1 = (uint64_t *) (PML1_OFFSET + (i->pml4i << PML3_SHIFT) + (i->pml3i << PML2_SHIFT) + (i->pml2i << PML1_SHIFT));

	return VY_OK;
}

vy_status_t mm_virtual_map(uintptr_t phys, uintptr_t virt, size_t pages, uintptr_t flags) {
	struct vm_indices i;

	mm_virtual_indices(&i, virt);

	if(!(i.pml4[i.pml4i] & PAGE_PRESENT)) {
		uintptr_t new_table = mm_physical_get();
		i.pml4[i.pml4i] = new_table | PAGE_PRESENT | PAGE_WRITE;

		memset(i.pml3, 0, 0x1000);
	}

	if(!(i.pml3[i.pml3i] & PAGE_PRESENT)) {
		uintptr_t new_table = mm_physical_get();
		i.pml3[i.pml3i] = new_table | PAGE_PRESENT | PAGE_WRITE;

		memset(i.pml2, 0, 0x1000);
	}

	if(!(i.pml2[i.pml2i] & PAGE_PRESENT)) {
		uintptr_t new_table = mm_physical_get();
		i.pml2[i.pml2i] = new_table | PAGE_PRESENT | PAGE_WRITE;

		memset(i.pml1, 0, 0x1000);
	}

	size_t c = i.pml1i;
	size_t mapped_pages = 0;

	for(c = i.pml1i; c < 512 && mapped_pages < pages; c++, mapped_pages++) {
		i.pml1[c] = (phys + (mapped_pages << PAGE_SHIFT)) | flags;
	}

	if(mapped_pages < pages) {
		mm_virtual_map(phys + (mapped_pages << PAGE_SHIFT), virt + (mapped_pages << PAGE_SHIFT), pages - mapped_pages, flags);
	}

	return VY_OK;
}

uintptr_t mm_virtual_get_phys(uintptr_t virt) {
	struct vm_indices i;
	uintptr_t addr = 0;

	mm_virtual_indices(&i, virt);

	if(i.pml4[i.pml4i] & PAGE_PRESENT && i.pml3[i.pml3i] & PAGE_PRESENT && i.pml2[i.pml2i] & PAGE_PRESENT && i.pml1[i.pml1i] & PAGE_PRESENT) {
		addr = (i.pml1[i.pml1i] & 0x000FFFFFFFFFF000);
	}

	return addr;
}

uintptr_t mm_virtual_get_flags(uintptr_t virt) {
	struct vm_indices i;
	uintptr_t flags = 0;

	mm_virtual_indices(&i, virt);

	if(i.pml4[i.pml4i] & PAGE_PRESENT && i.pml3[i.pml3i] & PAGE_PRESENT && i.pml2[i.pml2i] & PAGE_PRESENT && i.pml1[i.pml1i] & PAGE_PRESENT) {
		flags = (i.pml1[i.pml1i] & 0xfff0000000000fff);
	}

	return flags;
}

static void mm_virtual_insert_after(struct vm_node *after, struct vm_node *new) {
	if(!after || !new) {
		return;
	}

	new->next = after->next;
	new->prev = after;
	after->next = new;

	if(!new->next) {
		list.tail = new;
	} else {
		new->next->prev = new;
	}

	list.nodes++;
}

static vy_unused void mm_virtual_insert_before(struct vm_node *b, struct vm_node *new) {
	if(!b || !new) {
		return;
	}

	struct vm_node *a = b->prev;

	new->prev = a;
	new->next = b;

	if(new->prev) {
		a->next = new;
	} else {
		list.head = new;
	}

	b->prev = new;

	list.nodes++;
}

vy_unused static void mm_virtual_remove(struct vm_node *node) {
	if(!node) {
		return;
	}

	if(!node->prev) {
		list.head = node->next;
	} else {
		node->prev->next = node->next;
	}

	if(!node->next) {
		list.tail = node->prev;
	} else {
		node->next->prev = node->prev;
	}

	list.nodes--;
}

void mm_virtual_range_set(uintptr_t addr, size_t len, uintptr_t flags) {
	assert(!(addr & 0xFFF));
	assert(!(len & 0xFFF));

	/* find the node preceding the new one */
	struct vm_node *prev = list.head;

	while(prev && prev->next && prev->start <= addr) {
		prev = prev->next;
	}

	struct vm_node *next = (prev) ? prev->next : NULL;
	struct vm_node *new = mm_slab_alloc(cache);

	/* merge with surrounding nodes if applicable */

	if(!prev && !next) {
		list.head = new;
		list.tail = new;
		new->prev = NULL;
		new->next = NULL;
		new->start = addr;
		new->len = len;
		new->flags = flags;
		list.nodes = 1;

		return;
	}

	/* does the previous node overlap with the new one? */
	bool start_beyond_prev_end = (prev && prev->start + prev->len - 1 < addr);
	/* does the new node overlap with the beginning of the next? */
	bool end_before_next = (next && addr + len - 1 > next->start);

	bool within_prev = (prev && prev->start <= addr && (prev->start + prev->len - 1) >= (addr + len - 1));
	bool start_aliged = (prev && prev->start == addr);
	bool end_aliged = (prev && (prev->start + prev->len - 1) == (addr + len - 1));

	/* is the new node not within an existing one? */
	if(start_beyond_prev_end && end_before_next) {
		new->start = addr;
		new->len = len;
		new->flags = flags;

		if(prev) {
			mm_virtual_insert_after(prev, new);
		} else {
			list.head = new;
			new->prev = NULL;
			new->next = next;

			if(next) {
				next->prev = new;
			} else {
				list.tail = new;
			}

			list.nodes++;
		}

		return;
	} else if(within_prev && !start_aliged && !end_aliged) {
		struct vm_node *post = mm_slab_alloc(cache);
		size_t total_len = prev->len;

		prev->len = addr - prev->start;

		new->start = addr;
		new->len = len;
		new->flags = flags;

		post->start = addr + len;
		post->len = total_len - prev->len - len;

		mm_virtual_insert_after(prev, new);
		mm_virtual_insert_after(new, post);

		return;
	} else if(within_prev && start_aliged && !end_aliged) {
		/* start-aligned */
		new->start = addr + len;
		new->len = prev->len - len;
		new->flags = prev->flags;

		prev->len = len;
		prev->flags = flags;

		mm_virtual_insert_after(prev, new);
		return;
	} else if(within_prev && end_aliged && !start_aliged) {
		panic("end-aligned");
	} else if(within_prev && start_aliged && end_aliged) {
		panic("start-/end-aligned");
	}

	printf("prev: start=%#018lx, len=%#zx, %s\n", prev->start, prev->len, (prev->flags & VM_USED) ? "used" : "free");
	// printf("start_aliged=%u, end_aliged=%u, within_prev=%u, start_beyond_prev_end=%u, end_before_next=%u\n", start_aliged, end_aliged, within_prev, start_beyond_prev_end, end_before_next);
	panic("unhandled overlap addr=%#018lx len=%#zx flags=%#lx", addr, len, flags);
}

void mm_virtual_dump(void) {
	VM_FOREACH(node) {
		printf("vmm: %#018lx - %#018lx (%#zx pages) [%s]\n", node->start, node->start + node->len - 1, node->len >> 12, (node->flags & VM_USED) ? "used" : "free");
	}
}

uintptr_t mm_virtual_alloc(size_t pages) {
	if(!pages) {
		return 0;
	}

#ifdef CONFIG_VMM_DEBUG
	printf("vmm: %zu pages\n", pages);
#endif

	struct vm_node *alloc = NULL;
	size_t size = pages << 12;

	VM_FOREACH(node) {
		if((node->flags & VM_FREE) && node->len >= size) {
			alloc = node;
			break;
		}
	}

	if(!alloc) {
		return 0;
	}

	if(alloc->len == size) {
		alloc->flags &= ~VM_FREE;
		alloc->flags |= VM_USED;

#ifdef CONFIG_VMM_DEBUG
		mm_virtual_dump();
#endif
		return alloc->start;
	} else {
		struct vm_node *new = mm_slab_alloc(cache);

#ifdef CONFIG_VMM_DEBUG
		printf("vmm: alloc start = %#018lx, len = %zx [%s]\n", alloc->start, alloc->len, (alloc->flags & VM_USED) ? "used" : "free");
		if(alloc->next) printf("vmm: alloc->next start = %#018lx, len = %zx [%s]\n", alloc->next->start, alloc->next->len, (alloc->next->flags & VM_USED) ? "used" : "free");
#endif

		new->start = alloc->start + size;
		new->len = alloc->len - size;
		new->flags = alloc->flags;
#ifdef CONFIG_VMM_DEBUG
		printf("vmm: new start = %#018lx, len = %zx [%s]\n", new->start, new->len, (new->flags & VM_USED) ? "used" : "free");
#endif

		alloc->len = size;
		alloc->flags &= ~VM_FREE;
		alloc->flags |= VM_USED;

		mm_virtual_insert_after(alloc, new);

#ifdef CONFIG_VMM_DEBUG
		mm_virtual_dump();
#endif

		return alloc->start;
	}
}

size_t mm_virtual_free(uintptr_t addr) {
	struct vm_node *free = NULL;
	size_t ret = 0;

	VM_FOREACH(node) {
		if(node->start == addr && (node->flags & VM_USED)) {
			free = node;
			ret = node->len;
			break;
		}
	}

	if(!free) {
		return 0;
	}

	for(size_t i = 0; i < free->len; i += 0x1000) {
		mm_tlb_invlpg(addr + i);
	}

#ifdef CONFIG_VMM_DEBUG
	printf("vmm: freeing %#018lx (len %zu)\n", addr, free->len);
#endif

	free->flags &= ~VM_USED;
	free->flags |= VM_FREE;

	/* TODO: I'm probably just stupid but this breaks somewhere (i.e. infinite loop). Idea: fix this somehow */
	// if(free->prev && (free->prev->flags == free->flags)) {
	// 	uintptr_t tmp_start = free->prev->start;
	// 	size_t tmp_len = free->prev->len;
	// 	mm_virtual_remove(free->prev);
	// 	mm_slab_free(cache, free->prev);
	//
	// 	free->start = tmp_start;
	// 	free->len += tmp_len;
	// }
	//
	// if(free->next && (free->next->flags == free->flags)) {
	// 	size_t tmp_len = free->next->len;
	// 	mm_virtual_remove(free->next);
	// 	mm_slab_free(cache, free->next);
	//
	// 	free->len += tmp_len;
	// }

	return ret;
}

extern void *KERNEL_SIZE;

vy_status_t mm_virtual_init(void) {
	mm_page_tables_commit();

	cache = mm_cache_create_vm(sizeof(struct vm_node));

	mm_virtual_range_set(0xFFFFFE0000021000, 0x1FFFFFDF000, VM_FREE);
	mm_virtual_range_set(SLAB_VM_BASE, SLAB_SIZE, VM_USED);

	/* pmm stacks */
	mm_virtual_range_set(0xFFFFFEFFFFFFE000, 0x2000, VM_USED);
	/* page tables */
	mm_virtual_range_set(0xFFFFFF0000000000, 0x8000000000, VM_USED);
	/* kernel */
	mm_virtual_range_set(0xFFFFFFFF80000000, PAGE_UP((uintptr_t) &KERNEL_SIZE), VM_USED);

#ifdef CONFIG_VMM_DEBUG
	mm_virtual_dump();
#endif

	return VY_OK;
}
