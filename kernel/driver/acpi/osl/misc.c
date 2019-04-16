#include <acpi.h>
#include <stdio.h>
#include <vy.h>

uint64_t acpi_os_get_timer(void) {
	// panic("%s unimplemented", __func__);
	return 0;
}

acpi_status acpi_os_signal(uint32_t function, void *info vy_unused) {
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

acpi_status acpi_os_get_line(char *buffer vy_unused, uint32_t buffer_length vy_unused, uint32_t *bytes_read vy_unused) {
	panic("%s unimplemented", __func__);
}

/* TODO: allow for handlers to register? */
acpi_status acpi_os_enter_sleep(uint8_t state vy_unused, uint32_t a vy_unused, uint32_t b vy_unused) {
	return AE_OK;
}
