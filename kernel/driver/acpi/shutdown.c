#include <acpi.h>

void acpi_shutdown(void) {
	ACPI_STATUS status;

	status = AcpiEnterSleepStatePrep(ACPI_STATE_S5);
	if(ACPI_FAILURE(status)) {
		AcpiTerminate();
		panic("ACPI failure %u @ AcpiEnterSleepStatePrep\n", status);
	}

	__asm volatile ("cli");
	status = AcpiEnterSleepState(ACPI_STATE_S5);
	if(ACPI_FAILURE(status)) {
		AcpiTerminate();
		panic("ACPI failure %u @ AcpiEnterSleepState\n", status);
	}
}
