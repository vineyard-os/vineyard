#include <boot/info.h>
#include <driver/framebuffer.h>
#include <mm/page_tables.h>
#include <uefi.h>

void efi_init(void) {
	efi_status status;

	status = info.st->BootServices->SetWatchdogTimer(0, 0, 0, NULL);
	status = info.st->ConOut->ClearScreen(info.st->ConOut);

	framebuffer_init();

	status = efi_get_rsdp();
	EFIERR(status);

	status = efi_get_image(&info.image);
	EFIERR(status);

	mm_page_tables_setup();

	status = efi_get_memory_map();
	EFIERR(status);

	status = info.st->BootServices->ExitBootServices(info.handle, info.efi_memory_map_key);
	EFIERR(status);
}
