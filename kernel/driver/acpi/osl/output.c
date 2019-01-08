#include <acpi.h>
#include <stdio.h>
#include <vy.h>

no_warn(-Wformat-nonliteral)
no_warn_clang(-Wused-but-marked-unused)

void AcpiOsPrintf(const char *format, ...) {
	va_list args;
	va_start(args, format);

#ifdef ACPI_DEBUG
	vprintf(format, args);
#endif

	va_end(args);
}

void AcpiOsVprintf(vy_unused const char *format, vy_unused va_list args) {
#ifdef ACPI_DEBUG
	vprintf(format, args);
#endif
}

void AcpiOsRedirectOutput(vy_unused void *destination) {
	panic("%s unimplemented", __func__);
}
