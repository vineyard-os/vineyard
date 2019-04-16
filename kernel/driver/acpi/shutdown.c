#include <acpi.h>

void acpi_shutdown(void) {
	acpi_status status;

	status = acpi_enter_sleep_state_prep(ACPI_STATE_S5);
	if(ACPI_FAILURE(status)) {
		acpi_terminate();
		panic("ACPI failure %u @ AcpiEnterSleepStatePrep\n", status);
	}

	status = acpi_disable_all_gpes();
	if(ACPI_FAILURE(status)) {
		acpi_terminate();
		panic("ACPI failure %u @ AcpiDisableAllGpes\n", status);
	}

	asm volatile ("cli");

	status = acpi_enter_sleep_state(ACPI_STATE_S5);
	if(ACPI_FAILURE(status)) {
		acpi_terminate();
		panic("ACPI failure %u @ AcpiEnterSleepState\n", status);
	}
}
