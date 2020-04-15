#pragma once

#include <stdint.h>
#include <vy.h>

typedef struct {
    char signature[8];
    uint8_t checksum;
    char oem_id[6];
    uint8_t revision;
    uint32_t rsdt_addr;
    uint32_t length;
    uint64_t xsdt_addr;
    uint8_t extended_checksum;
    uint8_t reserved[3];
} vy_packed rsdp_t;

typedef struct {
    char signature[4];
    uint32_t length;
    uint8_t revision;
    uint8_t checksum;
    char oem_id[6];
    char oem_table_id[8];
    uint32_t oem_revision;
    uint32_t creator_id;
    uint32_t creator_revision;
    uint64_t sdt_pointer[0];
} vy_packed xsdt_t;

typedef struct {
	uint8_t type;
	uint8_t len;
	union {
		struct {
			uint8_t id;
			uint8_t apic_id;
			uint32_t flags;
		} vy_packed lapic;

		struct {
			uint8_t id;
			uint8_t reserved;
			uint32_t addr;
			uint32_t gsi_base;
		} vy_packed ioapic;

		struct {
			uint8_t bus;
			uint8_t irq;
			uint32_t gsi;
			uint16_t flags;
		} vy_packed intr;

		struct {
			uint16_t flags;
			uint32_t gsi;
		} vy_packed nmi;

		struct {
			uint8_t id;
			uint16_t flags;
			uint8_t lint;
		} vy_packed lnmi;

		struct {
			uint16_t reserved;
			uint64_t addr;
		} vy_packed lapic_addr;
	};
} vy_packed madt_entry_t;

typedef struct {
    char signature[4];
    uint32_t length;
    uint8_t revision;
    uint8_t checksum;
    char oem_id[6];
    char oem_table_id[8];
    uint32_t oem_revision;
    uint32_t creator_id;
    uint32_t creator_revision;
    uint32_t lapic_addr;
    uint32_t flags;
    madt_entry_t entries[0];
} vy_packed madt_t;