#include <driver/nvme.h>
#include <mm/virtual.h>
#include <string.h>
#include <stdlib.h>

nvme_id_controller_t *nvme_identify_controller(nvme_t *controller) {
	nvme_id_controller_t *id = memalign(0x1000, 0x1000);
	memset(id, 0, 0x1000);

	nvme_sq_entry_t *cmd = nvme_command_create(&controller->admin_sq, 6U, NULL, mm_virtual_get_phys(id));

	cmd->dword[10] = 1U;

	nvme_command_submit(&controller->admin_sq);
	nvme_cq_entry_t *cqe = nvme_wait(&controller->admin_sq);

	if(cqe->status >> 1) {
		panic("[NVMe]	controller id failed with status %x\n", cqe->status >> 1);
	}

	return id;
}

nvme_id_ns_t *nvme_identify_ns(nvme_t *controller, uint8_t ns_id) {
	nvme_id_ns_t *id = memalign(0x1000, 0x1000);
	memset(id, 0, 0x1000);

	nvme_sq_entry_t *cmd = nvme_command_create(&controller->admin_sq, 6U, NULL, mm_virtual_get_phys(id));

	cmd->ns_id = ns_id + 1;
	cmd->dword[10] = 0U;

	nvme_command_submit(&controller->admin_sq);
	nvme_cq_entry_t *cqe = nvme_wait(&controller->admin_sq);

	if(cqe->status >> 1) {
		panic("[NVMe]	ns id failed with status %x\n", cqe->status >> 1);
	}

	return id;
}
