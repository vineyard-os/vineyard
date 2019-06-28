#include <assert.h>
#include <driver/nvme.h>
#include <mm/virtual.h>
#include <stdlib.h>
#include <string.h>

void nvme_cq_init(nvme_t *controller, nvme_cq_t *cq, uint16_t index, uint16_t length) {
	assert((index & 1) == 1);

	memset(cq, 0, sizeof(*cq));

	cq->phase = 1;
	cq->doorbell = (volatile uint32_t *) (controller->base + 0x1000 + (index * controller->doorbell_stride));
	cq->mask = length - 1;
	cq->head = 0;
	cq->entry = memalign(0x1000, 0x1000);

	memset(cq->entry, 0, 0x1000);
}

void nvme_sq_init(nvme_t *controller, nvme_sq_t *sq, uint16_t index, uint16_t length, nvme_cq_t *cq) {
	assert((index & 1) == 0);

	memset(sq, 0, sizeof(*sq));

	sq->cq = cq;
	sq->doorbell = (volatile uint32_t *) (controller->base + 0x1000 + (index * controller->doorbell_stride));
	sq->mask = length - 1;
	sq->head = 0;
	sq->tail = 0;
	sq->entry = memalign(0x1000, 0x1000);

	memset(sq->entry, 0, 0x1000);
}

void nvme_io_cq_init(nvme_t *controller, nvme_cq_t *cq, uint16_t index) {
	uint32_t length = 1 + (controller->reg->cap & 0xFFFF);

	if(length > (0x1000 / sizeof(nvme_cq_entry_t))) {
		length = (0x1000 / sizeof(nvme_cq_entry_t));
	}

	nvme_cq_init(controller, cq, index, length);

	nvme_sq_entry_t *cmd = nvme_command_create(&controller->admin_sq, 5U, NULL, mm_virtual_get_phys((uintptr_t) cq->entry));

	cmd->dword[10] = (cq->mask << 16) | (index >> 1);
	cmd->dword[11] = 1;

	nvme_command_submit(&controller->admin_sq);
	nvme_cq_entry_t *cqe = nvme_wait(&controller->admin_sq);

	if(cqe->status >> 1) {
		panic("[NVMe]	creating I/O cq returned %u", cqe->status >> 1);
	}
}

void nvme_io_sq_init(nvme_t *controller, nvme_sq_t *sq, uint16_t index, nvme_cq_t *cq) {
	uint32_t length = 1 + (controller->reg->cap & 0xFFFF);

	if(length > (0x1000 / sizeof(nvme_cq_entry_t))) {
		length = (0x1000 / sizeof(nvme_cq_entry_t));
	}

	nvme_sq_init(controller, sq, index, length, cq);

	nvme_sq_entry_t *cmd = nvme_command_create(&controller->admin_sq, 1U, NULL, mm_virtual_get_phys((uintptr_t) sq->entry));

	cmd->dword[10] = (sq->mask << 16) | (index >> 1);
	cmd->dword[11] = (index >> 1) << 16 | 1;

	nvme_command_submit(&controller->admin_sq);
	nvme_cq_entry_t *cqe = nvme_wait(&controller->admin_sq);

	if(cqe->status >> 1) {
		panic("[NVMe]	creating I/O cq returned %u", cqe->status >> 1);
	}
}
