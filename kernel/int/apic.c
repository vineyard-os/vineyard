#include <assert.h>
#include <cpu/cpu.h>
#include <cpu/msr.h>
#include <int/apic.h>
#include <int/idt.h>
#include <int/isr.h>
#include <int/ioapic.h>
#include <int/pic.h>
#include <mm/virtual.h>
#include <time/pit.h>
#include <stdbool.h>
#include <stdio.h>
#include <vy.h>

#define LVT_MASKED         0x00010000
#define LVT_TYPE_FIXED     0x00000000
#define LVT_TYPE_SMI       0x00000200
#define LVT_TYPE_NMI       0x00000400
#define LVT_TYPE_EXTINT    0x00000700

#define LVT_TIMER_PERIODIC 0x00020000
#define LVT_TIMER_ONE_SHOT 0x00000000

#define MSR_APIC_BASE      0x0000001B

#define APIC_BASE_ENABLED 0x800
#define APIC_BASE_BSP     0x100

#define APIC_TPR        0x08
#define APIC_EOI        0x0B
#define APIC_SVR        0x0F
#define APIC_ESR        0x28

#define APIC_LVT_TIMER  0x32
#define APIC_LVT_LINT0  0x35
#define APIC_LVT_LINT1  0x36
#define APIC_LVT_ERROR  0x37

/* APIC timer initial count register */
#define APIC_TIMER_ICR  0x38
/* APIC timer current count register */
#define APIC_TIMER_CCR  0x39
/* APIC timer divide configuration register */
#define APIC_TIMER_DCR  0x3E

#define SVR_ENABLED 0x100

/* possible values for the APIC timer DCR */
#define DCR_1   0xB
#define DCR_2   0x0
#define DCR_4   0x1
#define DCR_8   0x2
#define DCR_16  0x3
#define DCR_32  0x8
#define DCR_64  0x9
#define DCR_128 0xA

static volatile uint32_t *apic_mmio;

static uint64_t apic_read(size_t reg) {
	return apic_mmio[reg << 2];
}

static void apic_write(size_t reg, uint32_t value) {
	apic_mmio[reg << 2] = value;
}

void apic_ack(void) {
	apic_write(APIC_EOI, 0);
}


void apic_init(void) {
	asm volatile ("cli");
}

void apic_enable(void) {
	cpu_t *cpu = cpu_get();

	uintptr_t apic_mmio_addr = mm_virtual_alloc(1);
	apic_mmio = (volatile uint32_t *) apic_mmio_addr;
	mm_virtual_map(cpu->lapic_addr & ~0xFFFUL, (uintptr_t) apic_mmio, 1, PAGE_PRESENT | PAGE_WRITE | PAGE_NX);

	uint64_t apic_base = (msr_read(MSR_APIC_BASE) & APIC_BASE_BSP) | (cpu->lapic_addr & 0xFFFFF000) | APIC_BASE_ENABLED;
	msr_write(MSR_APIC_BASE, apic_base);

	/* set the spurious interrupt vector */
	apic_write(APIC_SVR, 0);
	apic_write(APIC_SVR, SVR_ENABLED | 0xFF);

	/* reset the error status */
	apic_write(APIC_ESR, 0);
	apic_write(APIC_ESR, 0);

	apic_write(APIC_LVT_LINT0, cpu->apic_lint_nmi0 ? LVT_TYPE_NMI : LVT_MASKED);
	apic_write(APIC_LVT_LINT1, cpu->apic_lint_nmi1 ? LVT_TYPE_NMI : LVT_MASKED);
	apic_write(APIC_LVT_TIMER, LVT_MASKED);
	apic_write(APIC_LVT_ERROR, LVT_ERROR_INT);

	/* reset the priority so we accept all interrupts */
	apic_write(APIC_TPR, 0);

	/* ack any outstounding interrupts */
	apic_ack();

	apic_write(APIC_LVT_TIMER, LVT_MASKED);
	apic_write(APIC_TIMER_ICR, 0xFFFFFFFF);
	apic_write(APIC_TIMER_DCR, DCR_128);
	pit_wait(10);

	cpu->apic_timer_ticks_per_ms = (uint32_t) (0xFFFFFFFF - apic_read(APIC_TIMER_CCR)) * 128U / 10U;
}

void apic_timer_monotonic(uint32_t ms, isr_handler_t handler) {
	cpu_t *cpu = cpu_get();

	isr_register(LVT_TIMER_INT, handler);

	apic_write(APIC_LVT_TIMER, LVT_TIMER_PERIODIC | LVT_TYPE_FIXED | LVT_TIMER_INT);
	apic_write(APIC_TIMER_ICR, ms * cpu->apic_timer_ticks_per_ms / 16U);
	apic_write(APIC_TIMER_DCR, DCR_16);
}