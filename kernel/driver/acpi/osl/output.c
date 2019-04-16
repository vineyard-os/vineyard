#include <acpi.h>
#include <stdio.h>
#include <vy.h>

no_warn(-Wformat-nonliteral)
no_warn_clang(-Wused-but-marked-unused)

void acpi_os_printf(const char *format, ...) {
	va_list args;
	va_start(args, format);

#ifdef ACPI_DEBUG
	vprintf(format, args);
#endif

	va_end(args);
}

void acpi_os_vprintf(vy_unused const char *format, vy_unused va_list args) {
#ifdef ACPI_DEBUG
	vprintf(format, args);
#endif
}

void acpi_os_redirect_output(vy_unused void *destination) {
	panic("%s unimplemented", __func__);
}
