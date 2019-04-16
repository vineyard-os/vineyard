#include <acpi.h>
#include <int/isr.h>
#include <vy.h>

static acpi_osd_handler handler;
static void *context;

static void acpi_handler(vy_unused cpu_state_t *frame) {
	handler(context);
}

acpi_status acpi_os_install_interrupt_handler(uint32_t interrupt, acpi_osd_handler hand, void *ctx) {
	isr_register(interrupt + 32, acpi_handler);

	handler = hand;
	context = ctx;

	return AE_OK;
}

acpi_status acpi_os_remove_interrupt_handler(uint32_t interrupt, vy_unused acpi_osd_handler hand) {
	isr_unregister(interrupt + 32);

	return AE_OK;
}
