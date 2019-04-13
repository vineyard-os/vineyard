#include <acpi.h>
#include <int/apic.h>
#include <time/pit.h>
#include <stdbool.h>
#include <stdio.h>
#include <vy.h>

no_warn(-Wunused-parameter)
no_warn(-Wnull-pointer-arithmetic)

static ACPI_STATUS acpi_ec_handler(uint32_t function, ACPI_PHYSICAL_ADDRESS address, uint32_t bits, uint64_t *value, void *handler_context, void *region_context) {
	return AE_OK;
}

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

	status = AcpiInstallAddressSpaceHandler(ACPI_ROOT_OBJECT, ACPI_ADR_SPACE_EC, &acpi_ec_handler, NULL, NULL);
	if(ACPI_FAILURE(status)) {
		AcpiTerminate();
		panic("ACPI failure %u @ AcpiInstallAddressSpaceHandler", status);
	}

	status = AcpiEnableSubsystem(ACPI_FULL_INITIALIZATION);
	if(ACPI_FAILURE(status)) {
		AcpiTerminate();
		panic("ACPI failure %u @ AcpiEnableSubsystem", status);
	}

	status = AcpiInstallGlobalEventHandler(acpi_global_event_handler, NULL);
	if(ACPI_FAILURE(status)) {
		AcpiTerminate();
		panic("ACPI failure %u @ AcpiInstallGlobalEventHandler", status);
	}

	status = AcpiInitializeObjects(ACPI_FULL_INITIALIZATION);
	if(ACPI_FAILURE(status)) {
		AcpiTerminate();
		panic("ACPI failure %u @ AcpiInitializeObjects", status);
	}

	apic_enable();

	status = AcpiEnableEvent(ACPI_EVENT_POWER_BUTTON, 0);
	if(ACPI_FAILURE(status)) {
		AcpiTerminate();
		panic("ACPI failure %u @ AcpiEnableEvent", status);
	}

	pit_wait(10);
	asm volatile ("sti");
}
