#include <acpi.h>
#include <vy.h>

acpi_status acpi_os_create_mutex(vy_unused acpi_mutex *mutex) {
	return AE_OK;
}

void acpi_os_delete_mutex(vy_unused acpi_mutex mutex) {
	return;
}

acpi_status acpi_os_acquire_mutex(vy_unused acpi_mutex mutex, vy_unused uint16_t timeout) {
	return AE_OK;
}

void acpi_os_release_mutex(vy_unused acpi_mutex mutex) {
	return;
}

acpi_status acpi_os_create_semaphore(vy_unused uint32_t max_units, vy_unused uint32_t initial_units, acpi_semaphore *s) {
	*s = (acpi_handle) 1;
	return AE_OK;
}

acpi_status acpi_os_delete_semaphore(vy_unused acpi_semaphore s) {
	return AE_OK;
}

acpi_status acpi_os_wait_semaphore(vy_unused acpi_semaphore s, vy_unused uint32_t units, vy_unused uint16_t timeout) {
	panic("%s unimplemented", __func__);
}

acpi_status acpi_os_signal_semaphore(vy_unused acpi_semaphore s, vy_unused uint32_t units) {
	return AE_OK;
}

acpi_status acpi_os_create_lock(vy_unused acpi_spinlock *lock) {
	return AE_OK;
}

void acpi_os_delete_lock(vy_unused acpi_spinlock lock) {
	return;
}

acpi_cpu_flags acpi_os_acquire_lock(vy_unused acpi_spinlock lock) {
	return AE_OK;
}

void acpi_os_release_lock(vy_unused acpi_spinlock lock, vy_unused acpi_cpu_flags flags) {
	return;
}
