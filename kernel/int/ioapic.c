#include <assert.h>
#include <int/ioapic.h>
#include <mm/virtual.h>
#include <stdlib.h>
#include <stdio.h>
#include <vy.h>

#define IOAPIC_REDTBL 0x10U

static list_t ioapic_list;

static uint32_t ioapic_read(ioapic_t *apic, uint32_t reg) {
	*(apic->reg) = reg;
	asm volatile("" : : : "memory");
	return *(apic->val);
}

static void ioapic_write(ioapic_t *apic, uint32_t reg, uint32_t val) {
	*(apic->reg) = reg;
	asm volatile("" : : : "memory");
	*(apic->val) = val;
}

void ioapic_init(size_t id, uintptr_t addr, size_t irq_base) {
	ioapic_t *ioapic = malloc(sizeof(*ioapic));

	if(!ioapic) {
		return;
	}

	uintptr_t ioapic_mmio_addr = mm_virtual_alloc(1);
	volatile uint32_t *ioapic_mmio = (volatile uint32_t *) ioapic_mmio_addr;
	mm_virtual_map(addr, (uintptr_t) ioapic_mmio, 1, PAGE_PRESENT | PAGE_WRITE | PAGE_NX);

	ioapic->id = id;
	ioapic->reg = (uint32_t *) (ioapic_mmio_addr + (addr & 0xFFF));
	ioapic->val = (uint32_t *) (ioapic_mmio_addr + (addr & 0xFFF) + 16);
	ioapic->irq_base = irq_base;
	ioapic->irqs = ((ioapic_read(ioapic, 1) >> 16) & 0xFF) + 1;

	#ifdef CONFIG_ACPI_DEBUG
	printf("[apic]	IOAPIC id %zu (%018lx) for %zu IRQs starting at %zu\n", id, addr, ioapic->irqs, irq_base);
	#endif

	list_append_data(&ioapic_list, ioapic);
}

/* route an IRQ to an interrupt */
void ioapic_route(size_t from, uintptr_t flags, size_t to) {
	/* TODO: handle multiple IOAPICs */
	ioapic_t *ioapic = ioapic_list.head->data;

	assert(ioapic && ioapic->irq_base <= from && ioapic->irq_base + ioapic->irqs > from);

	uintptr_t redtbl_entry = (to & 0xFF) | flags;

	ioapic_write(ioapic, IOAPIC_REDTBL + (from << 1) + 1U, (redtbl_entry >> 32) & 0xFFFFFFFF);
	ioapic_write(ioapic, IOAPIC_REDTBL + (from << 1), redtbl_entry & 0xFFFFFFFF);
}
