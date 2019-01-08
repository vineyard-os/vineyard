#include <acpi.h>
#include <vy.h>

ACPI_STATUS AcpiOsReadPciConfiguration(ACPI_PCI_ID *id vy_unused, uint32_t reg vy_unused, UINT64 *val vy_unused, uint32_t width vy_unused) {
	panic("%s unimplemented", __func__);
}

ACPI_STATUS AcpiOsWritePciConfiguration(ACPI_PCI_ID *id vy_unused, uint32_t reg vy_unused, UINT64 val vy_unused, uint32_t width vy_unused) {
	panic("%s unimplemented", __func__);
}
