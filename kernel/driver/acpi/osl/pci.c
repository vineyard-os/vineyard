#include <acpi.h>
#include <cpu/ports.h>
#include <stdio.h>
#include <vy.h>

/* TODO: integrate this with actual PCI code */
acpi_status acpi_os_read_pci_configuration(struct acpi_pci_id *id, uint32_t reg, uint64_t *value, uint32_t width) {
	uint32_t reg_aligned = reg & ~0x03U;
	uint32_t offset = reg & 0x03U;
	uint32_t addr = (uint32_t) ((uint32_t) (id->bus << 16U) | (uint32_t) (id->device << 11U) | (uint32_t) (id->function << 8U) | (uint32_t) (1 << 31U) | reg_aligned);

	outl(0xCF8, addr);
	uint32_t ret = inl(0xCFC);

	void *res = (char *) &ret + offset;
	size_t count = width >> 3;
	*value = 0;

	memcpy(value, res, count);

	return AE_OK;
}

acpi_status acpi_os_write_pci_configuration(struct acpi_pci_id *id, uint32_t reg, uint64_t val, uint32_t width) {
	uint32_t addr = (uint32_t) ((uint32_t) (id->bus << 16U) | (uint32_t) (id->device << 11U) | (uint32_t) (id->function << 8U) | (uint32_t) (1 << 31U) | reg);

	outl(0xCF8, addr);

	switch(width) {
		case 8: {
			outb(0xCFC, val & 0xFF);
			break;
		}
		case 16: {
			outw(0xCFC, val & 0xFFFF);
			break;
		}
		case 32: {
			outl(0xCFC, val & 0xFFFFFFFF);
			break;
		}
	}

	return AE_OK;
}
