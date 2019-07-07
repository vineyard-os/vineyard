#include <fs/btrfs.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <vy.h>

/* check whether this partition is btrfs-formatted */
bool btrfs_check_partition(nvme_ns_t *ns, uint64_t start, size_t len vy_unused) {
	char *superblock = malloc(0x1000);

	if(!superblock) {
		free(superblock);
		return false;
	}

	nvme_read(ns, start + (64 * 1024), 0x1000, (char *) superblock);

	if(strncmp(&superblock[0x40], "_BHRfS_M", 8)) {
		free(superblock);
		return false;
	}

	free(superblock);

	return true;
}
