#include <boot/panic.h>
#include <driver/nvme.h>
#include <stdio.h>

void nvme_ns_probe(nvme_t *controller, uint8_t id) {
	if(id >= controller->namespaces) {
		panic("[NVMe]	invalid namespace id %u", id);
	}

	nvme_ns_t *ns = &controller->namespace_list[id];
	nvme_id_ns_t *info = nvme_identify_ns(controller, id);
	nvme_lba_format_t *format = &info->lba_format[info->formatted_lba_size & 0x0F];

	if(format->lba_data_size < 9) {
		panic("[NVMe]	invalid block size given: 2^%u = %u", format->lba_data_size, 1U << format->lba_data_size);
	}

	ns->controller = controller;
	ns->id = id;
	ns->lba_count = info->ns_capacity;
	ns->block_size = 1U << format->lba_data_size;
}
