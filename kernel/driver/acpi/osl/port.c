#include <acpi.h>
#include <cpu/ports.h>
#include <vy.h>

ACPI_STATUS AcpiOsReadPort(ACPI_IO_ADDRESS addr, uint32_t *value, uint32_t width) {
	uint16_t a = (uint16_t) addr;

	switch(width) {
		case 8: {
			*value = inb(a);
			break;
		}
		case 16: {
			*value = inw(a);
			break;
		}
		case 32: {
			*value = inl(a);
			break;
		}
		default: {
			return AE_BAD_PARAMETER;
		}
	}

	return AE_OK;
}

ACPI_STATUS AcpiOsWritePort(ACPI_IO_ADDRESS addr, uint32_t value, uint32_t width) {
	uint16_t a = (uint16_t) addr;

	switch(width) {
		case 8: {
			outb(a, (uint8_t) value);
			break;
		}
		case 16: {
			outw(a, (uint16_t) value);
			break;
		}
		case 32: {
			outl(a, value);
			break;
		}
		default: {
			return AE_BAD_PARAMETER;
		}
	}

	return AE_OK;
}
