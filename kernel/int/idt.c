#include <cpu/ports.h>
#include <int/idt.h>
#include <int/isr.h>
#include <mm/virtual.h>
#include <stdio.h>
#include <string.h>

struct idt_entry {
	uint16_t offset_low;
	uint16_t selector;
	uint8_t ist;
	uint8_t flags;
	uint16_t offset_mid;
	uint32_t offset_high;
	uint32_t zero;
} __attribute__((packed));

static struct idt_entry idt[256];
static idtr_t idtr;

static void idt_set(uint8_t entry, void (*handler)(void), uint8_t flags) {
	struct idt_entry *e = &idt[entry];

	e->selector = 0x08;

	uintptr_t addr = (uintptr_t) handler;
	e->offset_low = addr & 0xFFFF;
	e->offset_mid = (addr >> 16) & 0xFFFF;
	e->offset_high = (uint32_t) ((addr >> 32) & 0xFFFFFFFF);

	e->flags = flags;
}

void idt_init(void) {
	__asm volatile ("cli");

	memset(&idt, 0, sizeof(idt));

	idt_set(0, &isr_handle_0, 0x8E);
	idt_set(1, &isr_handle_1, 0x8E);
	idt_set(2, &isr_handle_2, 0x8E);
	idt_set(3, &isr_handle_3, 0x8E);
	idt_set(4, &isr_handle_4, 0x8E);
	idt_set(5, &isr_handle_5, 0x8E);
	idt_set(6, &isr_handle_6, 0x8E);
	idt_set(7, &isr_handle_7, 0x8E);
	idt_set(8, &isr_handle_8, 0x8E);
	idt_set(9, &isr_handle_9, 0x8E);
	idt_set(10, &isr_handle_10, 0x8E);
	idt_set(11, &isr_handle_11, 0x8E);
	idt_set(12, &isr_handle_12, 0x8E);
	idt_set(13, &isr_handle_13, 0x8E);
	idt_set(14, &isr_handle_14, 0x8E);
	idt_set(15, &isr_handle_15, 0x8E);
	idt_set(16, &isr_handle_16, 0x8E);
	idt_set(17, &isr_handle_17, 0x8E);
	idt_set(18, &isr_handle_18, 0x8E);
	idt_set(19, &isr_handle_19, 0x8E);
	idt_set(20, &isr_handle_20, 0x8E);
	idt_set(21, &isr_handle_21, 0x8E);
	idt_set(22, &isr_handle_22, 0x8E);
	idt_set(23, &isr_handle_23, 0x8E);
	idt_set(24, &isr_handle_24, 0x8E);
	idt_set(25, &isr_handle_25, 0x8E);
	idt_set(26, &isr_handle_26, 0x8E);
	idt_set(27, &isr_handle_27, 0x8E);
	idt_set(28, &isr_handle_28, 0x8E);
	idt_set(29, &isr_handle_29, 0x8E);
	idt_set(30, &isr_handle_30, 0x8E);
	idt_set(31, &isr_handle_31, 0x8E);

	idt_set(32, &isr_handle_irq0, 0x8E);
	idt_set(33, &isr_handle_irq1, 0x8E);
	idt_set(34, &isr_handle_irq2, 0x8E);
	idt_set(35, &isr_handle_irq3, 0x8E);
	idt_set(36, &isr_handle_irq4, 0x8E);
	idt_set(37, &isr_handle_irq5, 0x8E);
	idt_set(38, &isr_handle_irq6, 0x8E);
	idt_set(39, &isr_handle_irq7, 0x8E);
	idt_set(40, &isr_handle_irq8, 0x8E);
	idt_set(41, &isr_handle_irq9, 0x8E);
	idt_set(42, &isr_handle_irq10, 0x8E);
	idt_set(43, &isr_handle_irq11, 0x8E);
	idt_set(44, &isr_handle_irq12, 0x8E);
	idt_set(45, &isr_handle_irq13, 0x8E);
	idt_set(46, &isr_handle_irq14, 0x8E);
	idt_set(47, &isr_handle_irq15, 0x8E);
	idt_set(48, &isr_handle_irq16, 0x8E);
	idt_set(49, &isr_handle_irq17, 0x8E);
	idt_set(50, &isr_handle_irq18, 0x8E);
	idt_set(51, &isr_handle_irq19, 0x8E);
	idt_set(52, &isr_handle_irq20, 0x8E);
	idt_set(53, &isr_handle_irq21, 0x8E);
	idt_set(54, &isr_handle_irq22, 0x8E);
	idt_set(55, &isr_handle_irq23, 0x8E);

	idt_set(LVT_TIMER, &isr_handle_lvt_timer, 0x8E);
	idt_set(LVT_ERROR, &isr_handle_lvt_error, 0x8E);
	idt_set(0xFF, &isr_handle_spurious, 0x8E);

	idtr.base = (uintptr_t) idt;
	idtr.limit = sizeof(idt) - 1;

	idt_load(&idtr);
}
