#include <acpi.h>
#include <int/isr.h>
#include <vy.h>

static ACPI_OSD_HANDLER handler;
static void *context;

static void acpi_handler(vy_unused cpu_state_t *frame) {
	handler(context);
}

ACPI_STATUS AcpiOsInstallInterruptHandler(uint32_t interrupt, ACPI_OSD_HANDLER hand, void *ctx) {
	isr_register(interrupt + 32, acpi_handler);

	handler = hand;
	context = ctx;

	return AE_OK;
}

ACPI_STATUS AcpiOsRemoveInterruptHandler(uint32_t interrupt, vy_unused ACPI_OSD_HANDLER hand) {
	isr_unregister(interrupt + 32);

	return AE_OK;
}
