#include <assert.h>
#include <boot/panic.h>
#include <driver/nvme.h>
#include <mm/virtual.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void nvme_rw_read(nvme_ns_t *ns, uint64_t lba, uint16_t count, char *buf) {
	assert(((uintptr_t) buf & 0xFFF) == 0);

	nvme_sq_entry_t *cmd = nvme_command_create(&ns->controller->io_sq, 2U, 0, mm_virtual_get_phys((uintptr_t) buf));

	cmd->ns_id = ns->id + 1;
	cmd->dword[10] = lba & 0xFFFFFFFF;
	cmd->dword[11] = (lba >> 32) & 0xFFFFFFFF;
	cmd->dword[12] = count - 1U;

	nvme_command_submit(&ns->controller->io_sq);
	nvme_cq_entry_t *cqe = nvme_wait(&ns->controller->io_sq);

	if(cqe->status >> 1) {
		panic("[NVMe]	I/O read failed with status %#x", (cqe->status >> 1) & 0x7FFFU);
	}
}

void nvme_read(nvme_ns_t *ns, uint64_t physical, size_t length, char *buf) {
	assert(ns->block_size == 512);
	assert(!(length & 0x1FF));

	uint64_t lba = physical >> 9;
	size_t blocks = length >> 9;
	size_t left = length;

	if(((physical + length) >> 9) > (((lba << 9) + length) >> 9)) {
		blocks++;
	}

	char *local = memalign(0x1000, (length + 0xFFF) & ~0xFFF);

	for(size_t i = 0; i < blocks; i += 8) {
		nvme_rw_read(ns, lba + i, 8, local);
		memcpy(&buf[i << 9], local, min(left, 0x1000));

		if(left >= 0x1000) {
			left -= 0x1000;
		} else {
			left = 0;
			break;
		}
	}

	free(local);
}
