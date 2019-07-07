#include <driver/nvme.h>
#include <pci/config.h>
#include <mm/virtual.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

nvme_t *nvme_init(uint8_t bus, uint8_t slot, uint8_t function) {
	nvme_t *controller = calloc(1, sizeof(*controller));
	pci_bar_t bar;

	pci_config_bar(bus, slot, function, 0, &bar);

	/* allocate two pages; one for the registers, one for the doorbells */
	controller->base = mm_virtual_alloc(2);
	controller->reg = (nvme_reg_t volatile *) controller->base;
	mm_virtual_map(bar.addr, controller->base, 2, PAGE_PRESENT | PAGE_WRITE);

	controller->doorbell_stride = 4U << NVME_CAP_DSTRD(controller->reg->cap);

	/* 7.6.1 Initialization */
	pci_config_set_busmaster(bus, slot, function);

	controller->reg->cc = 0;
	nvme_wait_csts_rdy(controller, 0);

	nvme_cq_init(controller, &controller->admin_cq, 1, 0x1000 / sizeof(nvme_cq_entry_t));
	nvme_sq_init(controller, &controller->admin_sq, 0, 0x1000 / sizeof(nvme_sq_entry_t), &controller->admin_cq);

	controller->reg->aqa = controller->admin_cq.mask << 16 | controller->admin_sq.mask;
	controller->reg->asq = mm_virtual_get_phys((uintptr_t) controller->admin_sq.entry);
	controller->reg->acq = mm_virtual_get_phys((uintptr_t) controller->admin_cq.entry);
	controller->reg->intms = ~0;

	controller->reg->cc = 1 | (4U << 20) | (6U << 16);

	nvme_wait_csts_rdy(controller, 1);

	nvme_id_controller_t *controller_id = nvme_identify_controller(controller);

	controller->serial = strdup(controller_id->serial);
	controller->model = strdup(controller_id->model);
	controller->firmware = strdup(controller_id->firmware);
	controller->namespaces = controller_id->namespaces;
	controller->id = controller_id->controller_id;

	free(controller_id);

	controller->namespace_list = malloc(sizeof(nvme_ns_t) * controller->namespaces);

	nvme_ns_probe(controller, 0);

	nvme_io_cq_init(controller, &controller->io_cq, 3U);
	nvme_io_sq_init(controller, &controller->io_sq, 2U, &controller->io_cq);

	return controller;
}
