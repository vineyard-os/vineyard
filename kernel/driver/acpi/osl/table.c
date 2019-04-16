#include <acpi.h>
#include <vy.h>

acpi_status acpi_os_get_table_by_address(acpi_physical_address address vy_unused, struct acpi_table_header **out vy_unused) {
	return AE_SUPPORT;
}

acpi_status acpi_os_get_table_by_index(uint32_t index vy_unused, struct acpi_table_header **table vy_unused, uint32_t *instance vy_unused, acpi_physical_address *address vy_unused) {
	return AE_SUPPORT;
}

acpi_status acpi_os_get_table_by_name(char *signature vy_unused, uint32_t instance vy_unused, struct acpi_table_header **table vy_unused, acpi_physical_address *address vy_unused) {
	return AE_SUPPORT;
}
