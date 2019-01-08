#include <cpu/ports.h>
#include <int/pic.h>

#define PIC1_CMD  0x0020
#define PIC1_DATA 0x0021
#define PIC2_CMD  0x00A0
#define PIC2_DATA 0x00A1

#define ICW1_IC4  0x1
#define ICW1_SNGL 0x2
#define ICW1_ADI  0x4
#define ICW1_LTIM 0x8
#define ICW1_MB1  0x10 /* must be set to 1 */

#define ICW4_8086 0x1
#define ICW4_AEOI 0x2
#define ICW4_M    0x4  /* don't care if BUF not set */
#define ICW4_BUF  0x8
#define ICW4_SFNM 0x10

#define OCW2_EOI 0x20
#define OCW2_SL  0x40
#define OCW2_R   0x80

#define OCW3_RIS  0x1
#define OCW3_RR   0x2
#define OCW3_P    0x4
#define OCW3_MB1  0x8  /* must be set to 1 */
#define OCW3_SMM  0x20
#define OCW3_ESMM 0x40

void pic_init(void) {
	/* start initialising the PICs */
	outb(PIC1_CMD, ICW1_MB1 | ICW1_IC4);
	outb(PIC2_CMD, ICW1_MB1 | ICW1_IC4);

	/* set the offsets */
	outb(PIC1_DATA, 0x20);
	outb(PIC2_DATA, 0x28);

	/* tell the PICs how they are wired to each other */
	outb(PIC1_DATA, 0x04); /* IRQ2 has slave */
	outb(PIC2_DATA, 0x02); /* connected to IRQ2 of master */

	/* change the PICs to use 8086 mode */
	outb(PIC1_DATA, ICW4_8086 | ICW4_M);
	outb(PIC2_DATA, ICW4_8086);

	/* mask every IRQ by default except IRQ2 which is used for cascading */
	outb(PIC1_DATA, 0xFB);
	outb(PIC2_DATA, 0xFF);
}
