#include <cpu/ports.h>
#include <time/pit.h>
#include <stddef.h>
#include <stdint.h>

#define PORT_CMD	0x0043
#define PORT_CH0	0x0040
#define PORT_CH1	0x0041
#define PORT_CH2	0x0042

#define CMD_BINARY	0x00
#define CMD_BCD		0x01
#define CMD_MODE0	0x00 /* interrupt on terminal count */
#define CMD_MODE1	0x02 /* hardware re-triggerable one-shot */
#define CMD_MODE2	0x04 /* rate generator */
#define CMD_MODE3	0x06 /* square wave generator */
#define CMD_MODE4	0x08 /* software triggered strobe */
#define CMD_MODE5	0x09 /* hardware triggered strobe */

#define CMD_ACC_LATCH	0x00
#define CMD_ACC_LO		0x20
#define CMD_ACC_HI		0x10
#define CMD_ACC_LOHI	0x30
#define CMD_CH0			0x00
#define CMD_CH1			0x40
#define CMD_CH2			0x80
#define CMD_READBACK	0xC0

/* readback commands fpr PORT_CMD */
#define CMD_RB_CH0				0x02
#define CMD_RB_CH1				0x04
#define CMD_RB_CH2				0x08
#define CMD_RB_NO_STATUS_LATCH	0x10
#define CMD_RB_NO_COUNT_LATCH	0x20

/* readback status byte */
#define STATUS_BINARY		0x00
#define STATUS_BCD			0x01
#define STATUS_MODE0		0x00
#define STATUS_MODE1		0x02
#define STATUS_MODE2		0x04
#define STATUS_MODE3		0x06
#define STATUS_MODE4		0x08
#define STATUS_MODE5		0x0A
#define STATUS_MODE2_ALT	0x0C
#define STATUS_MODE3_ALT	0x0E
#define STATUS_OUTPUT		0x80

void pit_wait(size_t ms) {
	/* 1193 ticks per ms */
	uint64_t total_count = 0x4A9UL * ms;

	do {
		uint16_t count = (total_count > 0xFFFFU) ? 0xFFFFU : (uint16_t) total_count;

		outb(PORT_CMD, CMD_ACC_LOHI);
		outb(PORT_CH0, (uint8_t) (count & 0xFF));
		outb(PORT_CH0, (uint8_t) (count >> 8));

		do {
			asm volatile ("pause");
			outb(PORT_CMD, CMD_READBACK | CMD_RB_CH0 | CMD_RB_NO_COUNT_LATCH);
		} while((inb(PORT_CH0) & STATUS_OUTPUT) == 0);

		total_count -= count;
	} while((total_count & ~0xFFFFU) != 0);
}
