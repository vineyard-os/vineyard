#pragma once

#include <acpica/acpi.h>
#include <vy.h>

warn_pop

acpi_status AcpiDbSingleStep(void *state, void *op, uint32_t optype);
void AcpiDbSignalBreakPoint(void *state);
void AcpiDbDumpMethodInfo(acpi_status status, void *state);
void AcpiDbDisplayArgumentObject(void *desc, void *state);
void AcpiDbDisplayResultObject(void *desc, void *walk);

void acpi_init(void);
void acpi_shutdown(void);
