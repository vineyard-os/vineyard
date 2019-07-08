#include <fs/btrfs.h>
#include <stdlib.h>
#include <string.h>
#include <vy.h>

/* check whether this partition is btrfs-formatted */
bool fat32_check_partition(nvme_ns_t *ns, uint64_t start, size_t len vy_unused) {
	char vy_free *sector = malloc(512);

	if(!sector) {
		free(sector);
		return false;
	}

	nvme_read(ns, start + 512, 512, (char *) sector);

	if(strncmp(sector, "RRaA", 4) || strncmp(&sector[0x1E4], "rrAa", 4)) {
		return false;
	}

	return true;
}
