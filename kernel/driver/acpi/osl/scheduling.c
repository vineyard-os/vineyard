#include <acpi.h>
#include <time/pit.h>
#include <stdio.h>
#include <vy.h>

acpi_thread_id acpi_os_get_thread_id(void) {
	return 1;
}

acpi_status acpi_os_execute(vy_unused acpi_execute_type Type, vy_unused acpi_osd_exec_callback Function, vy_unused void *Context) {
	panic("%s unimplemented", __func__);
}

void acpi_os_sleep(uint64_t Milliseconds) {
	pit_wait(Milliseconds);
}

void acpi_os_stall(uint32_t Microseconds) {
	if(Microseconds > 1000) {
		pit_wait(Microseconds / 1000);
	} else {
		panic("%s(%u us) unimplemented", __func__, Microseconds);
	}
}

void acpi_os_wait_events_complete(void) {
	/* TODO: figure out what this is supposed to do exactly */
}
