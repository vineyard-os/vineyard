#include <assert.h>
#include <boot/panic.h>
#include <driver/nvme.h>
#include <string.h>

nvme_sq_entry_t *nvme_command_create(nvme_sq_t *sq, uint8_t opcode, uintptr_t metadata, uintptr_t data) {
	assert((data & 0xFFF) == 0);

	nvme_sq_entry_t *sqe = &sq->entry[sq->tail];
	memset(sqe, 0, sizeof(*sqe));

	sqe->cmd = opcode | (sq->tail << 16);
	sqe->metadata_ptr = metadata;
	sqe->data_prp1 = data;

	return sqe;
}

void nvme_command_submit(nvme_sq_t *sq) {
	sq->tail = (sq->tail + 1) & sq->mask;

	barrier();

	*sq->doorbell = sq->tail;
}

nvme_cq_entry_t *nvme_command_cqe(nvme_sq_t *sq) {
	nvme_cq_t *cq = sq->cq;

	if(!nvme_poll_cq(cq)) {
		panic("[NVMe]	no completion queue entry ready");
	}

	nvme_cq_entry_t *cqe = &cq->entry[cq->head];
	if(((cq->head + 1) & cq->mask) != cq->head + 1) {
		cq->phase = ~cq->phase;
	}

	cq->head = (cq->head + 1) & cq->mask;

	if(cqe->sq_head != sq->head) {
		sq->head = cqe->sq_head;
	}

	barrier();
	*cq->doorbell = cq->head;

	return cqe;
}
