#include <boot/info.h>
#include <driver/framebuffer.h>
#include <mm/page_tables.h>
#include <uefi.h>

void efi_init(void) {
	framebuffer_init();
}
