#include <acpi.h>
#include <cpu/ports.h>
#include <stdio.h>
#include <vy.h>

/* TODO: integrate this with actual PCI code */
ACPI_STATUS AcpiOsReadPciConfiguration(ACPI_PCI_ID *id, uint32_t reg, UINT64 *value, uint32_t width) {
	uint32_t reg_aligned = reg & ~0x03U;
	uint32_t offset = reg & 0x03U;
	uint32_t addr = (uint32_t) ((uint32_t) (id->Bus << 16U) | (uint32_t) (id->Device << 11U) | (uint32_t) (id->Function << 8U) | (uint32_t) (1 << 31U) | reg_aligned);

	outl(0xCF8, addr);
	uint32_t ret = inl(0xCFC);

	void *res = (char *) &ret + offset;
	size_t count = width >> 3;
	*value = 0;

	memcpy(value, res, count);

	return AE_OK;
}

ACPI_STATUS AcpiOsWritePciConfiguration(ACPI_PCI_ID *id vy_unused, uint32_t reg vy_unused, UINT64 val vy_unused, uint32_t width vy_unused) {
	panic("%s unimplemented", __func__);
}
