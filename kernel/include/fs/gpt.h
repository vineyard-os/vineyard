#pragma once

#include <driver/nvme.h>

typedef struct {
	uint8_t guid[16];
	uint8_t parition_guid[16];
	uint64_t start_lba;
	uint64_t end_lba;
	uint64_t attr;
	uint16_t name[36];
} __attribute__((packed)) gpt_partition_t;

typedef struct {
	uint8_t signature[8];
	uint32_t revision;
	uint32_t header_size;
	uint32_t crc32;
	uint32_t reserved;
	uint64_t current_lba;
	uint64_t backup_lba;
	uint64_t partitions_lba_start;
	uint64_t partitions_lba_end;
	uint8_t guid[16];
	uint64_t partition_entries_lba;
	uint32_t partition_entries_count;
	uint32_t partition_entry_size;
	uint32_t partition_entries_crc32;
	uint8_t zero[420];
	gpt_partition_t entries[128];
} __attribute__((packed)) gpt_header_t;

void gpt_init(nvme_ns_t *ns);
