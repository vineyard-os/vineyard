#include <assert.h>
#include <acpi.h>
#include <acpi/types.h>
#include <boot/info.h>
#include <cpu/cpu.h>
#include <int/apic.h>
#include <int/ioapic.h>
#include <int/pic.h>
#include <stdio.h>

#define MADT_TYPE_LAPIC      0x00
#define MADT_TYPE_IOAPIC     0x01
#define MADT_TYPE_INTR       0x02
#define MADT_TYPE_NMI        0x03
#define MADT_TYPE_LNMI       0x04
#define MADT_TYPE_LAPIC_ADDR 0x05

static void acpi_parse_madt(madt_t *madt) {
	cpu_t *cpu = cpu_get();
	uintptr_t end = (uintptr_t) madt + madt->length;

	cpu->lapic_addr = madt->lapic_addr;
	uint32_t madt_flags = madt->flags;
	bool bsp_apic = false;

	uintptr_t ptr = (uintptr_t) &madt->entries;

	if(madt_flags & ACPI_MADT_PCAT_COMPAT) {
		pic_init();
	}

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
				cpu->lapic_addr = entry->lapic_addr.addr;
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

	ptr = (uintptr_t) &madt->entries;

	while(ptr < end) {
		madt_entry_t *entry = (void *) ptr;

		if(!entry->len) {
			break;
		}

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
}

void acpi_tables_init(void) {
	rsdp_t *rsdp = info.rsdp;
	xsdt_t *xsdt = (void *) (rsdp->xsdt_addr & 0xFFFFFFFF);
	size_t xsdt_entries = ((xsdt->length - sizeof(xsdt_t)) / 8);

	for(size_t i = 0; i < xsdt_entries; i++) {
		struct acpi_table_header *header = (void *) xsdt->sdt_pointer[i];

		if(!strncmp(header->signature, "APIC", 4)) {
			madt_t *madt = (void *) header;

			uint8_t checksum = 0;

			for(size_t j = 0; j < madt->length; j++) {
				checksum += ((uint8_t *) madt)[j];
			}

			assert(checksum == 0);

			acpi_parse_madt(madt);
		}
	}
}