#include <acpi.h>
#include <vy.h>

ACPI_STATUS AcpiOsGetTableByAddress(ACPI_PHYSICAL_ADDRESS address vy_unused, ACPI_TABLE_HEADER **out vy_unused) {
	return AE_SUPPORT;
}

ACPI_STATUS AcpiOsGetTableByIndex(uint32_t index vy_unused, ACPI_TABLE_HEADER **table vy_unused, uint32_t *instance vy_unused, ACPI_PHYSICAL_ADDRESS *address vy_unused) {
	return AE_SUPPORT;
}

ACPI_STATUS AcpiOsGetTableByName(char *signature vy_unused, uint32_t instance vy_unused, ACPI_TABLE_HEADER **table vy_unused, ACPI_PHYSICAL_ADDRESS *address vy_unused) {
	return AE_SUPPORT;
}
