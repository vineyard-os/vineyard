#pragma once

#include <acpi.h>

void acpi_ec_init(void);
acpi_status acpi_ec_handler(uint32_t function, acpi_physical_address address, uint32_t bits, uint64_t *value, void *handler_context, void *region_context);
