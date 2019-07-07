#include <fs/fat32.h>
#include <fs/gpt.h>
#include <fs/btrfs.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void gpt_init(nvme_ns_t *ns) {
	gpt_header_t *header = memalign(0x1000, sizeof(*header));

	nvme_read(ns, 512, sizeof(*header), (char *) header);

	if(strncmp("EFI PART", (char *) header->signature, 8)) {
		free(header);
		panic("[GPT]	no header found");
	}

	for(size_t i = 0; i < header->partition_entries_count; i++) {
		uint64_t start = header->entries[i].start_lba << 9;
		uint64_t end = (header->entries[i].end_lba + 1) << 9;

		if(start) {
			printf("partition %lu, %lu MiB, start %lu, end %lu", i + 1, (end - start) >> 20, header->entries[i].start_lba, header->entries[i].end_lba);
		} else {
			break;
		}

		if(btrfs_check_partition(ns, start, end - start)) {
			printf(", btrfs");
		} else if(fat32_check_partition(ns, start, end - start)) {
			printf(", fat32");
		}

		printf("\n");
	}

	free(header);
}
