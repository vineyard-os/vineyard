#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define LFB(x, y) (((x) * framebuffer_bpp) + ((y) * framebuffer_pitch))

extern uint32_t framebuffer_font_lookup[2];
extern const uint8_t framebuffer_font[46384];
extern const uint16_t framebuffer_font_index[11120];

extern uint32_t *framebuffer_lfb;
extern size_t framebuffer_lfb_size;
extern uint32_t framebuffer_width;
extern uint32_t framebuffer_height;
extern uint32_t framebuffer_pitch;
extern uint32_t framebuffer_bpp;
extern bool framebuffer_ready;

void framebuffer_init(void);
void framebuffer_set(size_t h, size_t v, uint32_t color);
size_t framebuffer_draw_char(const char *c, size_t x, size_t y);

size_t terminal_putc(const char *c);
