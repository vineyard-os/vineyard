#include <acpi.h>
#include <boot/info.h>
#include <stdio.h>
#include <vy.h>

ACPI_STATUS AcpiOsInitialize(void) {
	return AE_OK;
}

ACPI_STATUS AcpiOsTerminate(void) {
	return AE_OK;
}

ACPI_PHYSICAL_ADDRESS AcpiOsGetRootPointer(void) {
	return (uintptr_t) info.rsdp;
}

ACPI_STATUS AcpiOsPredefinedOverride(vy_unused const ACPI_PREDEFINED_NAMES *PredefinedObject, ACPI_STRING *NewValue) {
	*NewValue = 0;
	return AE_OK;
}

ACPI_STATUS AcpiOsTableOverride(vy_unused ACPI_TABLE_HEADER *ExistingTable, ACPI_TABLE_HEADER **NewTable) {
	*NewTable = 0;
	return AE_OK;
}

ACPI_STATUS AcpiOsPhysicalTableOverride(vy_unused ACPI_TABLE_HEADER *ExistingTable, ACPI_PHYSICAL_ADDRESS *NewAddress, vy_unused UINT32 *NewTableLength) {
	*NewAddress = 0;
	return AE_OK;
}

ACPI_STATUS AcpiDbSingleStep(void *state vy_unused, void *op vy_unused, uint32_t optype vy_unused) {
	panic("%s unimplemented", __func__);
}

void AcpiDbSignalBreakPoint(void *state vy_unused) {
	panic("%s unimplemented", __func__);
}

void AcpiDbDumpMethodInfo(ACPI_STATUS status vy_unused, void *state vy_unused) {
	panic("%s unimplemented", __func__);
}

void AcpiDbDisplayArgumentObject(void *desc vy_unused, void *state vy_unused) {
	panic("%s unimplemented", __func__);
}

void AcpiDbDisplayResultObject(void *desc vy_unused, void *walk vy_unused) {
	panic("%s unimplemented", __func__);
}
