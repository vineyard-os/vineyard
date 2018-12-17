#include <driver/uart.h>
#include <driver/framebuffer.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

/* variables in [px] */
static uint32_t terminal_x = 0;
static uint32_t terminal_y = 0;
static uint32_t terminal_char_width = 8;
static uint32_t terminal_char_height = 16;

/* variables in [lines] */
static uint32_t terminal_rows = 0;
static uint32_t terminal_lines = 0;

static void terminal_refresh(void) {
	/* check if we have set up our terminal sizes */
	if(terminal_rows == 0 && terminal_lines == 0) {
		terminal_rows = framebuffer_width / terminal_char_width;
		terminal_lines = framebuffer_height / terminal_char_height;
	}

	uint32_t bytes_per_line = framebuffer_pitch * terminal_char_height;

	/* check whether we have to wrap to next line */
	if(terminal_x >= framebuffer_width) {
		terminal_x = 0;
		terminal_y += terminal_char_height;
	}

	/* check whether we have to scroll */
	if((terminal_y + terminal_char_height) >= framebuffer_height) {
		uint8_t *start = (uint8_t *) framebuffer_lfb;

		for(size_t line = 0; line < terminal_lines - 1; line++) {
			memcpy(start, start + bytes_per_line, bytes_per_line);

			start += bytes_per_line;
		}

		memset(start, 0, bytes_per_line);

		terminal_y -= terminal_char_height;
	}
}

size_t terminal_putc(const char *c) {
	if(c[0] == '\n') {
		terminal_x = 0;
		terminal_y += terminal_char_height;

		goto exit;
	} else if(c[0] == '\t') {
		/* move terminal_x to the next-highest multiple of 4 */
		terminal_x = (terminal_x + (terminal_char_width << 2)) & ~((terminal_char_width << 2) - 1);

		goto exit;
	} else if(c[0] == '\b') {
		/* move the x coordinate 1 character to the left, but not off the screen */
		terminal_x = (terminal_x <= terminal_char_width) ? 0 : terminal_x - terminal_char_width;
		/* clear the character we deleted */
		framebuffer_draw_char(" ", terminal_x, terminal_y);

		/* for some reason, my GNOME terminal needs two backspaces to move back one char */
		uart_putc(0x08);
		uart_putc(0x08);
		/* we want a destructive backspace, so set it up ... */
		uart_putc(0x20);

		/* ... and complete it, including cleanup */
		goto exit;
	}

	terminal_refresh();
	size_t ret = framebuffer_draw_char(c, terminal_x, terminal_y);
	terminal_x += terminal_char_width;

	return ret;
exit:
	terminal_refresh();
	uart_putc((uint8_t) c[0]);
	return 1;
}
