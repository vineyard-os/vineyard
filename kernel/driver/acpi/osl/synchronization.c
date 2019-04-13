#include <acpi.h>
#include <vy.h>

ACPI_STATUS AcpiOsCreateMutex(vy_unused ACPI_MUTEX *mutex) {
	return AE_OK;
}

void AcpiOsDeleteMutex(vy_unused ACPI_MUTEX mutex) {
	return;
}

ACPI_STATUS AcpiOsAcquireMutex(vy_unused ACPI_MUTEX mutex, vy_unused uint16_t timeout) {
	return AE_OK;
}

void AcpiOsReleaseMutex(vy_unused ACPI_MUTEX mutex) {
	return;
}

ACPI_STATUS AcpiOsCreateSemaphore(vy_unused uint32_t max_units, vy_unused uint32_t initial_units, ACPI_SEMAPHORE *s) {
	*s = (ACPI_HANDLE) 1;
	return AE_OK;
}

ACPI_STATUS AcpiOsDeleteSemaphore(vy_unused ACPI_SEMAPHORE s) {
	return AE_OK;
}

ACPI_STATUS AcpiOsWaitSemaphore(vy_unused ACPI_SEMAPHORE s, vy_unused uint32_t units, vy_unused uint16_t timeout) {
	panic("%s unimplemented", __func__);
}

ACPI_STATUS AcpiOsSignalSemaphore(vy_unused ACPI_SEMAPHORE s, vy_unused uint32_t units) {
	return AE_OK;
}

ACPI_STATUS AcpiOsCreateLock(vy_unused ACPI_SPINLOCK *lock) {
	return AE_OK;
}

void AcpiOsDeleteLock(vy_unused ACPI_SPINLOCK lock) {
	return;
}

ACPI_CPU_FLAGS AcpiOsAcquireLock(vy_unused ACPI_SPINLOCK lock) {
	return AE_OK;
}

void AcpiOsReleaseLock(vy_unused ACPI_SPINLOCK lock, vy_unused ACPI_CPU_FLAGS flags) {
	return;
}
