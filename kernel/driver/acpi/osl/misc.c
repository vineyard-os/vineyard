#include <acpi.h>
#include <stdio.h>
#include <vy.h>

UINT64 AcpiOsGetTimer(void) {
	// panic("%s unimplemented", __func__);
	return 0;
}

ACPI_STATUS AcpiOsSignal(uint32_t function, void *info vy_unused) {
	switch(function) {
		case ACPI_SIGNAL_FATAL: {
			panic("[acpi]	AML fatal opcode");
			break;
		}
		case ACPI_SIGNAL_BREAKPOINT: {
			printf("[acpi]	AML breakpoint");
			break;
		}
		default: {
			return AE_BAD_PARAMETER;
		}
	}

	return AE_OK;
}

ACPI_STATUS AcpiOsGetLine(char *buffer vy_unused, uint32_t buffer_length vy_unused, uint32_t *bytes_read vy_unused) {
	panic("%s unimplemented", __func__);
}

/* TODO: allow for handlers to register? */
ACPI_STATUS AcpiOsEnterSleep(uint8_t state vy_unused, uint32_t a vy_unused, uint32_t b vy_unused) {
	return AE_OK;
}
