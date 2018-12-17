#pragma once

#include <stddef.h>
#include <stdint.h>

#define LFB(x, y) (((x) * framebuffer_bpp) + ((y) * framebuffer_pitch))

uint32_t framebuffer_font_lookup[2];
const uint8_t framebuffer_font[46384];
const uint16_t framebuffer_font_index[11120];

uint32_t *framebuffer_lfb;
size_t framebuffer_lfb_size;
uint32_t framebuffer_width;
uint32_t framebuffer_height;
uint32_t framebuffer_pitch;
uint32_t framebuffer_bpp;

void framebuffer_init(void);
void framebuffer_set(size_t h, size_t v, uint32_t color);
size_t framebuffer_draw_char(const char *c, size_t x, size_t y);

size_t terminal_putc(const char *c);
