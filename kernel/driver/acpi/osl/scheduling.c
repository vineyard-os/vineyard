#include <acpi.h>
#include <vy.h>

ACPI_THREAD_ID AcpiOsGetThreadId(void) {
	return 1;
}

ACPI_STATUS AcpiOsExecute(vy_unused ACPI_EXECUTE_TYPE Type, vy_unused ACPI_OSD_EXEC_CALLBACK Function, vy_unused void *Context) {
	panic("%s unimplemented", __func__);
}

void AcpiOsSleep(vy_unused UINT64 Milliseconds) {
	panic("%s unimplemented", __func__);
}

void AcpiOsStall(vy_unused UINT32 Microseconds) {
	panic("%s unimplemented", __func__);
}

void AcpiOsWaitEventsComplete(void) {
	/* TODO: figure out what this is supposed to do exactly */
}
