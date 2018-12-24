#include <boot/info.h>
#include <driver/framebuffer.h>
#include <driver/uart.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <uefi.h>

static size_t offset_1;
static size_t offset_2;
static size_t offset_3;
static size_t offset_4;
static size_t offset_5;
static size_t offset_6;
static size_t offset_7;

uint32_t *framebuffer_lfb = NULL;
size_t framebuffer_lfb_size = 0;
uint32_t framebuffer_width = 0;
uint32_t framebuffer_height = 0;
uint32_t framebuffer_pitch = 0;
uint32_t framebuffer_bpp = 0;
bool framebuffer_ready = false;

static bool framebuffer_setup(efi_graphics_output_protocol *gop) {
	size_t max_w = 0;
	size_t max_h = 0;
	uint32_t selection = 0;
	efi_status status;

	for(uint32_t i = 0; i < gop->Mode->MaxMode; i++) {
		efi_graphics_output_mode_information *mode_info;
		size_t info_size = sizeof(mode_info);
		info.st->BootServices->AllocatePool(EfiLoaderData, info_size, (void **) &mode_info);

		status = gop->QueryMode(gop, i, &info_size, &mode_info);
		EFIERR(status);

		size_t width = mode_info->HorizontalResolution;
		size_t h = mode_info->VerticalResolution;

		if(!memcmp(EFI_OVMF_SIGNATURE, info.st->FirmwareVendor, 13) && width == 800 && h == 600) {
			gop->SetMode(gop, i);

			return true;
		} else if(width > max_w || h > max_h) {
			max_w = width;
			max_h = h;
			selection = i;
		}
	}

	if(memcmp(EFI_OVMF_SIGNATURE, info.st->FirmwareVendor, 13) && selection != 0) {
		gop->SetMode(gop, selection);

		return true;
	}

	return false;
}

void framebuffer_init(void) {
	efi_graphics_output_protocol *gop;

	efi_gop_get(&gop);
	framebuffer_setup(gop);

	efi_graphics_output_mode_information *mode_info = gop->Mode->Info;

	framebuffer_lfb = (uint32_t *) gop->Mode->FrameBufferBase;
	framebuffer_width = mode_info->HorizontalResolution;
	framebuffer_height = mode_info->VerticalResolution;
	framebuffer_bpp = 4;
	framebuffer_pitch = mode_info->PixelsPerScanLine * framebuffer_bpp;
	framebuffer_lfb_size = gop->Mode->FrameBufferSize;

	offset_1 = framebuffer_bpp;
	offset_2 = framebuffer_bpp << 1;
	offset_3 = offset_1 + offset_2;
	offset_4 = framebuffer_bpp << 2;
	offset_5 = offset_4 + offset_1;
	offset_6 = offset_4 + offset_2;
	offset_7 = offset_4 + offset_3;

	framebuffer_ready = true;
}

void framebuffer_set(size_t h, size_t v, uint32_t color) {
	uint32_t *addr = framebuffer_lfb + LFB(h, v);
	*addr = color;
}

size_t framebuffer_draw_char(const char *c, size_t x, size_t y) {
	char *addr = (char *) framebuffer_lfb + LFB(x, y);
	uint32_t index = 0;
	size_t ret = 0;

	char c0 = (char) (c[0] & 0xFF);

	/* handle UTF-8 multi-byte characters correctly */
	if((c0 & 0x80) == 0) {
		/* single-byte */
		ret = 1;
		index = (uint32_t) c0;

		uart_putc((uint8_t) c[0]);
	} else if((c0 & 0xE0) == 0xC0) {
		/* 2 bytes */
		char c1 = c[1] & 0x3F;
		ret = 2;
		index = (uint32_t) (((c0 & 0x1F) << 6) | c1);

		uart_putc((uint8_t) c[0]);
		uart_putc((uint8_t) c[1]);
	} else if((c0 & 0xF0) == 0xE0) {
		/* 3 bytes */
		char c1 = (c[1] & 0xFF) & 0x3F;
		char c2 = (c[2] & 0xFF) & 0x3F;
		ret = 3;
		index = (uint32_t) (((c0 & 0x0F) << 12) | (c1 << 6) | c2);

		uart_putc((uint8_t) c[0]);
		uart_putc((uint8_t) c[1]);
		uart_putc((uint8_t) c[2]);
	} else if((c0 & 0xF8) == 0xF0) {
		/* 4 bytes */
		char c1 = (c[1] & 0xFF) & 0x3F;
		char c2 = (c[2] & 0xFF) & 0x3F;
		char c3 = (c[3] & 0xFF) & 0x3F;
		ret = 4;
		index = (uint32_t) (((c0 & 0x07) << 18) | (c1 << 12) | (c2 << 6) | c3);

		uart_putc((uint8_t) c[0]);
		uart_putc((uint8_t) c[1]);
		uart_putc((uint8_t) c[2]);
		uart_putc((uint8_t) c[3]);
	}

	if(index >= 64256) {
		index = (uint32_t) (framebuffer_font_index[index-54416] << 4);
	} else {
		index = (uint32_t) (framebuffer_font_index[index] << 4);
	}

	uint8_t *char_data = (uint8_t *) &framebuffer_font[index];
	uint8_t row_data;

	uint32_t mask_0;
	uint32_t mask_1;
	uint32_t mask_2;
	uint32_t mask_3;
	uint32_t mask_4;
	uint32_t mask_5;
	uint32_t mask_6;
	uint32_t mask_7;

	uint32_t fg = 0xFFFFFFFF;
	uint32_t bg = 0x00000000;

	for(int row = 0; row < 16; row++) {
		row_data = char_data[row];

		/* the leftmost pixel is represented by the MSB */
		mask_0 = framebuffer_font_lookup[(row_data >> 7) & 1];
		mask_1 = framebuffer_font_lookup[(row_data >> 6) & 1];
		mask_2 = framebuffer_font_lookup[(row_data >> 5) & 1];
		mask_3 = framebuffer_font_lookup[(row_data >> 4) & 1];
		mask_4 = framebuffer_font_lookup[(row_data >> 3) & 1];
		mask_5 = framebuffer_font_lookup[(row_data >> 2) & 1];
		mask_6 = framebuffer_font_lookup[(row_data >> 1) & 1];
		mask_7 = framebuffer_font_lookup[(row_data >> 0) & 1];

		*(uint32_t *) addr = (fg & mask_0) | (bg & ~mask_0);
		*(uint32_t *) (&addr[offset_1]) = (fg & mask_1) | (bg & ~mask_1);
		*(uint32_t *) (&addr[offset_2]) = (fg & mask_2) | (bg & ~mask_2);
		*(uint32_t *) (&addr[offset_3]) = (fg & mask_3) | (bg & ~mask_3);
		*(uint32_t *) (&addr[offset_4]) = (fg & mask_4) | (bg & ~mask_4);
		*(uint32_t *) (&addr[offset_5]) = (fg & mask_5) | (bg & ~mask_5);
		*(uint32_t *) (&addr[offset_6]) = (fg & mask_6) | (bg & ~mask_6);
		*(uint32_t *) (&addr[offset_7]) = (fg & mask_7) | (bg & ~mask_7);

		addr += framebuffer_pitch;
	}

	return ret;
}

uint32_t framebuffer_font_lookup[2] = {
    0x00000000,
    0xFFFFFFFF,
};
