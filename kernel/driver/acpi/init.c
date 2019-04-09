#include <acpi.h>
#include <int/apic.h>
#include <stdbool.h>
#include <stdio.h>
#include <vy.h>

static void acpi_global_event_handler(UINT32 type, vy_unused ACPI_HANDLE device, UINT32 number, vy_unused void* context) {
	if(type == ACPI_EVENT_TYPE_FIXED && number == ACPI_EVENT_POWER_BUTTON) {
		acpi_shutdown();
	}
}

void acpi_init(void) {
	ACPI_STATUS status;

	status = AcpiInitializeSubsystem();
	if(ACPI_FAILURE(status)) {
		AcpiTerminate();
		panic("ACPI failure %u @ AcpiInitializeSubsystem", status);
	}

	status = AcpiInitializeTables(NULL, 32, true);
	if(ACPI_FAILURE(status)) {
		AcpiTerminate();
		panic("ACPI failure %u @ AcpiInitializeTables", status);
	}

	status = AcpiLoadTables();
	if(ACPI_FAILURE(status)) {
		AcpiTerminate();
		panic("ACPI failure %u @ AcpiLoadTables", status);
	}

	apic_init();

	if(ACPI_FAILURE(status)) {
		AcpiTerminate();
		panic("ACPI failure %u @ AcpiInstallNotifyHandler", status);
	}

	status = AcpiEnableSubsystem(0);
	if(ACPI_FAILURE(status)) {
		AcpiTerminate();
		panic("ACPI failure %u @ AcpiEnableSubsystem", status);
	}

	status = AcpiInitializeObjects(ACPI_FULL_INITIALIZATION);
	if(ACPI_FAILURE(status)) {
		AcpiTerminate();
		panic("ACPI failure %u @ AcpiInitializeObjects", status);
	}

	apic_enable();
	__asm volatile ("sti");

	status = AcpiInstallGlobalEventHandler(acpi_global_event_handler, NULL);
	if(ACPI_FAILURE(status)) {
		AcpiTerminate();
		panic("ACPI failure %u @ AcpiInstallGlobalEventHandler", status);
	}

	status = AcpiEnableEvent(ACPI_EVENT_POWER_BUTTON, 0);
	if(ACPI_FAILURE(status)) {
		AcpiTerminate();
		panic("ACPI failure %u @ AcpiEnableEvent", status);
	}
}
