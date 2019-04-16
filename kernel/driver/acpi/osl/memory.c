#include <acpi.h>
#include <vy.h>

acpi_status acpi_os_read_memory(vy_unused acpi_physical_address addr, vy_unused uint64_t *value, vy_unused uint32_t width) {
	panic("%s unimplemented", __func__);
}

acpi_status acpi_os_write_memory(vy_unused acpi_physical_address addr, vy_unused uint64_t value, vy_unused uint32_t width) {
	panic("%s unimplemented", __func__);
}
