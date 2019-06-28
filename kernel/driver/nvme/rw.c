#include <assert.h>
#include <boot/panic.h>
#include <driver/nvme.h>
#include <mm/virtual.h>
#include <stdio.h>

void nvme_rw_read(nvme_ns_t *ns, uint64_t lba, char *buf) {
	assert(((uintptr_t) buf & 0xFFF) == 0);

	nvme_sq_entry_t *cmd = nvme_command_create(&ns->controller->io_sq, 2U, 0, mm_virtual_get_phys((uintptr_t) buf));

	cmd->ns_id = ns->id + 1;
	cmd->dword[10] = lba & 0xFFFFFFFF;
	cmd->dword[11] = (lba >> 32) & 0xFFFFFFFF;
	cmd->dword[12] = 1U << 31;

	nvme_command_submit(&ns->controller->io_sq);
	nvme_cq_entry_t *cqe = nvme_wait(&ns->controller->io_sq);

	if(cqe->status >> 1) {
		panic("[NVMe]	I/O read failed with status %#x", cqe->status >> 1);
	}
}
