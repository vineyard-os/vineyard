#include <acpi.h>
#include <acpi/ec.h>
#include <acpi/irq.h>
#include <int/apic.h>
#include <time/pit.h>
#include <stdbool.h>
#include <stdio.h>
#include <vy.h>

no_warn(-Wunused-parameter)
no_warn_clang(-Wnull-pointer-arithmetic)

static void acpi_global_event_handler(uint32_t type, vy_unused acpi_handle device, uint32_t number, vy_unused void* context) {
	if(type == ACPI_EVENT_TYPE_FIXED && number == ACPI_EVENT_POWER_BUTTON) {
		acpi_shutdown();
	}
}

static acpi_status acpi_walk_devices(acpi_handle object, uint32_t nesting_level, void *context, void **return_value) {
	struct acpi_device_info *dev_info;
	acpi_status status = acpi_get_object_info(object, &dev_info);

	if(ACPI_FAILURE(status)) {
		panic("ACPI failure %u @ acpi_get_object_info", status);
		return AE_ERROR;
	}

	acpi_irq_process(dev_info, object);

	return AE_OK;
}

void acpi_init(void) {
	acpi_status status;

	status = acpi_initialize_subsystem();
	if(ACPI_FAILURE(status)) {
		acpi_terminate();
		panic("ACPI failure %u @ AcpiInitializeSubsystem", status);
	}

	status = acpi_initialize_tables(NULL, 32, true);
	if(ACPI_FAILURE(status)) {
		acpi_terminate();
		panic("ACPI failure %u @ AcpiInitializeTables", status);
	}

	status = acpi_load_tables();
	if(ACPI_FAILURE(status)) {
		acpi_terminate();
		panic("ACPI failure %u @ AcpiLoadTables", status);
	}

	apic_init();

	status = acpi_enable_subsystem(ACPI_FULL_INITIALIZATION);
	if(ACPI_FAILURE(status)) {
		acpi_terminate();
		panic("ACPI failure %u @ AcpiEnableSubsystem", status);
	}

	acpi_ec_init();

	status = acpi_install_global_event_handler(acpi_global_event_handler, NULL);
	if(ACPI_FAILURE(status)) {
		acpi_terminate();
		panic("ACPI failure %u @ AcpiInstallGlobalEventHandler", status);
	}

	status = acpi_initialize_objects(ACPI_FULL_INITIALIZATION);
	if(ACPI_FAILURE(status)) {
		acpi_terminate();
		panic("ACPI failure %u @ AcpiInitializeObjects", status);
	}

	struct acpi_object_list        ArgList;
	union acpi_object             Arg[1];

	ArgList.count = 1;
	ArgList.pointer = Arg;

	Arg[0].type = ACPI_TYPE_INTEGER;
	Arg[0].integer.value = 1;

	status = acpi_evaluate_object(NULL, (acpi_string) "\\_PIC", &ArgList, NULL);
	if(ACPI_FAILURE(status) && status != AE_NOT_FOUND) {
		acpi_terminate();
		panic("ACPI failure %u @ AcpiEvaluateObject(_PIC)", status);
	}

	apic_enable();

	status = acpi_enable_event(ACPI_EVENT_POWER_BUTTON, 0);
	if(ACPI_FAILURE(status)) {
		acpi_terminate();
		panic("ACPI failure %u @ AcpiEnableEvent", status);
	}

	asm volatile ("sti");

	status = acpi_get_devices(NULL, acpi_walk_devices, NULL, NULL);
	if(ACPI_FAILURE(status)) {
		acpi_terminate();
		panic("ACPI failure %u @ AcpiGetDevices", status);
	}
}
