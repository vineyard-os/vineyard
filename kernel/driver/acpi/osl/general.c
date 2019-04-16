#include <acpi.h>
#include <boot/info.h>
#include <stdio.h>
#include <vy.h>

acpi_status acpi_os_initialize(void) {
	return AE_OK;
}

acpi_status acpi_os_terminate(void) {
	return AE_OK;
}

acpi_physical_address acpi_os_get_root_pointer(void) {
	return (uintptr_t) info.rsdp;
}

acpi_status acpi_os_predefined_override(const struct acpi_predefined_names *PredefinedObject, acpi_string *NewValue) {
	if(!PredefinedObject || !NewValue) {
		return AE_BAD_PARAMETER;
	}

	*NewValue = NULL;
	return AE_OK;
}

acpi_status acpi_os_table_override(vy_unused struct acpi_table_header *ExistingTable, struct acpi_table_header **NewTable) {
	*NewTable = 0;
	return AE_OK;
}

acpi_status acpi_os_physical_table_override(vy_unused struct acpi_table_header *ExistingTable, acpi_physical_address *NewAddress, vy_unused uint32_t *NewTableLength) {
	*NewAddress = 0;
	return AE_OK;
}

acpi_status AcpiDbSingleStep(void *state vy_unused, void *op vy_unused, uint32_t optype vy_unused) {
	panic("%s unimplemented", __func__);
}

void AcpiDbSignalBreakPoint(void *state vy_unused) {
	panic("%s unimplemented", __func__);
}

void AcpiDbDumpMethodInfo(acpi_status status vy_unused, void *state vy_unused) {
	panic("%s unimplemented", __func__);
}

void AcpiDbDisplayArgumentObject(void *desc vy_unused, void *state vy_unused) {
	panic("%s unimplemented", __func__);
}

void AcpiDbDisplayResultObject(void *desc vy_unused, void *walk vy_unused) {
	panic("%s unimplemented", __func__);
}
