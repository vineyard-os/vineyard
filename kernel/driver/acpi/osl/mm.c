#include <acpi.h>
#include <assert.h>
#include <mm/virtual.h>
#include <stdio.h>
#include <stdlib.h>

void *AcpiOsMapMemory(ACPI_PHYSICAL_ADDRESS PhysicalAddress, ACPI_SIZE length) {
	size_t overhang = (PhysicalAddress & 0xFFF);
	length = ALIGN_UP((length + overhang), 0x1000);
	uintptr_t virt = mm_virtual_alloc(length >> 12);
	uintptr_t phys = PhysicalAddress - overhang;

	for(size_t i = 0; i < length; i += 0x1000) {
		mm_virtual_map(phys + i, virt + i, 1, PAGE_PRESENT | PAGE_WRITE | PAGE_NX);
	}

	return (void *) (virt + overhang);
}

void AcpiOsUnmapMemory(void *where, ACPI_SIZE length) {
	uintptr_t addr = (uintptr_t) where & ~0xFFFUL;
	size_t len = ALIGN_UP(length, 0x1000);
	assert(len == mm_virtual_free(addr));
}

ACPI_STATUS AcpiOsGetPhysicalAddress(void *LogicalAddress, ACPI_PHYSICAL_ADDRESS *PhysicalAddress) {
	*PhysicalAddress = mm_virtual_get_phys((uintptr_t) LogicalAddress);

	return AE_OK;
}

void *AcpiOsAllocate(ACPI_SIZE size) {
	return malloc(size);
}

void AcpiOsFree(void *Memory) {
	free(Memory);
}

BOOLEAN AcpiOsReadable(vy_unused void *Memory, vy_unused ACPI_SIZE Length) {
	panic("%s unimplemented", __func__);
}

BOOLEAN AcpiOsWritable(vy_unused void *Memory, vy_unused ACPI_SIZE Length) {
	panic("%s unimplemented", __func__);
}
