#pragma once

#include <ctype.h>
#include <stdint.h>
#include <string.h>

#define ACPI_MACHINE_WIDTH          64
#define COMPILER_DEPENDENT_INT64    int64_t
#define COMPILER_DEPENDENT_UINT64   uint64_t

#define ACPI_USE_DO_WHILE_0
#define ACPI_MUTEX_TYPE             ACPI_OSL_MUTEX

#define ACPI_CACHE_T                ACPI_MEMORY_LIST
#define ACPI_USE_LOCAL_CACHE        1

#define ACPI_SINGLE_THREADED
#define ACPI_DEBUG_OUTPUT
#define ACPI_USE_STANDARD_HEADERS
#define ACPI_USE_SYSTEM_CLIBRARY

#include <acpica/platform/acgcc.h>
