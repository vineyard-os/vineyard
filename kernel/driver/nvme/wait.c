#include <boot/panic.h>
#include <driver/nvme.h>
#include <time/pit.h>

void nvme_wait_csts_rdy(nvme_t *controller, uint8_t rdy) {
	size_t timeouts = NVME_CAP_TO(controller->reg->cap);
	uint32_t csts;

	for(size_t i = 0; i < timeouts; i++) {
		if(((csts = controller->reg->csts) & 1) == rdy) {
			return;
		}

		pit_wait(500);
	}

	panic("[NVMe]	CSTS.RDY != %u after timeout", rdy);
}

int nvme_poll_cq(nvme_cq_t *cq) {
	uint32_t dword = cq->entry[cq->head].dword[3];
	barrier();
	return (!!(dword & (1U << 16)) == cq->phase);
}

nvme_cq_entry_t *nvme_wait(nvme_sq_t *sq) {
	for(size_t i = 0; i < 500; i++) {
		if(nvme_poll_cq(sq->cq)) {
			return nvme_command_cqe(sq);
		}

		pit_wait(10);
	}

	panic("[NVMe]	timed out waiting for command");
}
