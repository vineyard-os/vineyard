#include <acpi.h>
#include <vy.h>

ACPI_STATUS AcpiOsReadMemory(vy_unused ACPI_PHYSICAL_ADDRESS addr, vy_unused UINT64 *value, vy_unused UINT32 width) {
	panic("%s unimplemented", __func__);
}

ACPI_STATUS AcpiOsWriteMemory(vy_unused ACPI_PHYSICAL_ADDRESS addr, vy_unused UINT64 value, vy_unused UINT32 width) {
	panic("%s unimplemented", __func__);
}
