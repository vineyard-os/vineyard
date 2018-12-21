#include <cpu/ports.h>
#include <stdint.h>

uint8_t inb(uint16_t port) {
	uint8_t ret;

	__asm volatile ("inb %1, %0" : "=a" (ret) : "Nd" (port));

	return ret;
}

uint16_t inw(uint16_t port) {
	uint16_t ret;

	__asm volatile ("inw %1, %0" : "=a" (ret) : "Nd" (port));

	return ret;
}

uint32_t inl(uint16_t port) {
	uint32_t ret;

	__asm volatile ("inl %1, %0" : "=a" (ret) : "Nd" (port));

	return ret;
}

void outb(uint16_t port, uint8_t val) {
	__asm volatile ("outb %0, %1" : : "a" (val), "Nd" (port));
}

void outw(uint16_t port, uint16_t val) {
	__asm volatile ("outw %0, %1" : : "a" (val), "Nd" (port));
}

void outl(uint16_t port, uint32_t val) {
	__asm volatile ("outl %0, %1" : : "a" (val), "Nd" (port));
}
