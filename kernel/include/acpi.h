#pragma once

#include <acpica/acpi.h>
#include <vy.h>

warn_pop

acpi_status AcpiDbSingleStep(void *state, void *op, uint32_t optype);
void AcpiDbSignalBreakPoint(void *state);
void AcpiDbDumpMethodInfo(acpi_status status, void *state);
void AcpiDbDisplayArgumentObject(void *desc, void *state);
void AcpiDbDisplayResultObject(void *desc, void *walk);

typedef struct {
	char signature[ACPI_NAMESEG_SIZE];
	uint32_t length;
	uint8_t revision;
	uint8_t checksum;
	char oem_id[ACPI_OEM_ID_SIZE];
	char oem_table_id[ACPI_OEM_TABLE_ID_SIZE];
	uint32_t oem_revision;
	char asl_compiler_id[ACPI_NAMESEG_SIZE];
	uint32_t asl_compiler_revision;
	uint64_t reserved;
} vy_packed acpi_mcfg_t;

void acpi_init(void);
void acpi_shutdown(void);
