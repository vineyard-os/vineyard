#include <acpi.h>
#include <assert.h>
#include <cpu/cpu.h>
#include <cpu/msr.h>
#include <int/apic.h>
#include <int/idt.h>
#include <int/ioapic.h>
#include <int/pic.h>
#include <mm/virtual.h>
#include <stdbool.h>
#include <stdio.h>
#include <vy.h>

#define LVT_MASKED         0x00010000
#define LVT_TYPE_FIXED     0x00000000
#define LVT_TYPE_SMI       0x00000200
#define LVT_TYPE_NMI       0x00000400
#define LVT_TYPE_EXTINT    0x00000700

#define MADT_TYPE_LAPIC      0x00
#define MADT_TYPE_IOAPIC     0x01
#define MADT_TYPE_INTR       0x02
#define MADT_TYPE_NMI        0x03
#define MADT_TYPE_LNMI       0x04
#define MADT_TYPE_LAPIC_ADDR 0x05

#define MSR_APIC_BASE      0x0000001B

#define APIC_BASE_ENABLED 0x800
#define APIC_BASE_BSP     0x100

#define APIC_TPR        0x08
#define APIC_EOI        0x0B
#define APIC_SVR        0x0F
#define APIC_ESR        0x28

#define APIC_LVT_TIMER  0x32
#define APIC_LVT_LINT0  0x35
#define APIC_LVT_LINT1  0x36
#define APIC_LVT_ERROR  0x37

#define SVR_ENABLED 0x100

typedef struct {
	uint8_t type;
	uint8_t len;
	union {
		struct {
			uint8_t id;
			uint8_t apic_id;
			uint32_t flags;
		} vy_packed lapic;

		struct {
			uint8_t id;
			uint8_t reserved;
			uint32_t addr;
			uint32_t gsi_base;
		} vy_packed ioapic;

		struct {
			uint8_t bus;
			uint8_t irq;
			uint32_t gsi;
			uint16_t flags;
		} vy_packed intr;

		struct {
			uint16_t flags;
			uint32_t gsi;
		} vy_packed nmi;

		struct {
			uint8_t id;
			uint16_t flags;
			uint8_t lint;
		} vy_packed lnmi;

		struct {
			uint16_t reserved;
			uint64_t addr;
		} vy_packed lapic_addr;
	};
} vy_packed madt_entry_t;

typedef struct {
	uint32_t lapic_addr;
	uint32_t flags;
} vy_packed madt_extended_t;

static uintptr_t lapic_addr;
static volatile uint32_t *apic_mmio;

vy_unused static uint64_t apic_read(size_t reg) {
	return apic_mmio[reg << 2];
}

static void apic_write(size_t reg, uint32_t value) {
	apic_mmio[reg << 2] = value;
}

void apic_ack(void) {
	apic_write(APIC_EOI, 0);
}

void apic_init(void) {
	struct acpi_table_header *madt;
	acpi_status status = acpi_get_table((char *) "APIC", 0, &madt);
	assert(status == AE_OK && madt);

	asm volatile ("cli");

	uintptr_t end = (uintptr_t) madt + madt->length;
	madt_extended_t *madt_ext = (madt_extended_t *) ((uintptr_t) madt + sizeof(struct acpi_table_header));
	lapic_addr = madt_ext->lapic_addr;
	uint32_t madt_flags = madt_ext->flags;
	bool bsp_apic = false;

	uintptr_t ptr = (uintptr_t) madt + sizeof(struct acpi_table_header) + sizeof(madt_extended_t);

	while(ptr < end) {
		madt_entry_t *entry = (madt_entry_t *) ptr;

		switch(entry->type) {
			case MADT_TYPE_LAPIC: {
				if(entry->lapic.flags & 1) {
					uint8_t id = entry->lapic.id;
					uint8_t apic_id = entry->lapic.apic_id;

#ifdef CONFIG_ACPI_DEBUG
					printf("[apic]	LAPIC: id %hhu, apic_id %hhu\n", id, apic_id);
#endif
					if(!bsp_apic) {
						bsp_apic = true;
						cpu_t *cpu = cpu_get();
						cpu->apic_id = apic_id;
						cpu->acpi_id = id;
					}
				}
				break;
			}
			case MADT_TYPE_IOAPIC: {
				uint8_t id = entry->ioapic.id;
				uint32_t addr = entry->ioapic.addr;
				uint32_t gsi_base = entry->ioapic.gsi_base;

				ioapic_init(id, addr, gsi_base);
				break;
			}
			case MADT_TYPE_NMI: {
				puts("MADT_TYPE_NMI");
				break;
			}
			case MADT_TYPE_LNMI: {
				uint8_t id = entry->lnmi.id;
				uint8_t lint = entry->lnmi.lint;
				cpu_t *cpu = cpu_get();

				if(id == 0 || id == 0xFF) {
					if(lint == 0) {
						cpu->apic_lint_nmi0 = true;
					} else {
						cpu->apic_lint_nmi1 = true;
					}
				}

				break;
			}
			case MADT_TYPE_LAPIC_ADDR: {
				lapic_addr = entry->lapic_addr.addr;
				break;
			}
			case MADT_TYPE_INTR: {
				/* we deal with this in our second pass */
				break;
			}
			default: {
				printf("[madt]	unknown type %#x\n", entry->type);
				break;
			}
		}

		ptr += entry->len;
	}

	ptr = (uintptr_t) madt + sizeof(struct acpi_table_header) + sizeof(madt_extended_t);

	while(ptr < end) {
		madt_entry_t *entry = (madt_entry_t *) ptr;

		switch(entry->type) {
			case MADT_TYPE_INTR: {
				uint8_t irq = entry->intr.irq;
				uint32_t gsi = entry->intr.gsi;
				uint16_t flags = entry->intr.flags;
#ifdef CONFIG_ACPI_DEBUG
				uint8_t bus = entry->intr.bus;
				printf("[apic]	Interrupt Source Override: bus %hhu, IRQ source %hhu, Global System Interrupt %u, active %s %s\n", bus, irq, gsi, (flags & 2) ? "low" : "high", (flags & 8) ? "level" : "edge");
#endif
				uintptr_t irq_flags = REDTBL_DESTMODE_PHYSICAL | REDTBL_DELMODE_FIXED;

				/* the SCI interrupt is always active low, level triggered (ACPI 5.0 Errata A, section 5.2.9, table 5-34, p. 114) */
				if((flags & ACPI_MADT_POLARITY_MASK) == ACPI_MADT_POLARITY_ACTIVE_HIGH || (flags & ACPI_MADT_POLARITY_MASK) == ACPI_MADT_POLARITY_CONFORMS) {
					if(gsi != acpi_gbl_FADT.sci_interrupt) {
						irq_flags |= REDTBL_ACTIVE_HIGH;
					}
				}

				if((flags & ACPI_MADT_TRIGGER_MASK) == ACPI_MADT_TRIGGER_LEVEL || gsi == acpi_gbl_FADT.sci_interrupt) {
					irq_flags |= REDTBL_TRIGGER_LEVEL;
				}

				ioapic_route(irq, irq_flags, gsi + 32);
				break;
			}
			default: {
				break;
			}
		}

		ptr += entry->len;
	}

	if(madt_flags & ACPI_MADT_PCAT_COMPAT) {
		pic_init();
	}
}

void apic_enable(void) {
	cpu_t *cpu = cpu_get();

	uintptr_t apic_mmio_addr = mm_virtual_alloc(1);
	apic_mmio = (volatile uint32_t *) apic_mmio_addr;
	mm_virtual_map(lapic_addr & ~0xFFFUL, (uintptr_t) apic_mmio, 1, PAGE_PRESENT | PAGE_WRITE | PAGE_NX);

	uint64_t apic_base = (msr_read(MSR_APIC_BASE) & APIC_BASE_BSP) | (lapic_addr & 0xFFFFF000) | APIC_BASE_ENABLED;
	msr_write(MSR_APIC_BASE, apic_base);

	/* set the spurious interrupt vector */
	apic_write(APIC_SVR, SVR_ENABLED | 0xFF);

	/* reset the error status */
	apic_write(APIC_ESR, 0);
	apic_write(APIC_ESR, 0);

	apic_write(APIC_LVT_LINT0, cpu->apic_lint_nmi0 ? LVT_TYPE_NMI : LVT_MASKED);
	apic_write(APIC_LVT_LINT1, cpu->apic_lint_nmi1 ? LVT_TYPE_NMI : LVT_MASKED);
	apic_write(APIC_LVT_TIMER, LVT_MASKED);
	apic_write(APIC_LVT_ERROR, LVT_ERROR);

	/* reset the priority so we accept all interrupts */
	apic_write(APIC_TPR, 0);

	/* ack any outstounding interrupts */
	apic_ack();
}
