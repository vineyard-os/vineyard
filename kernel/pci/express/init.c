#include <acpi.h>
#include <assert.h>
#include <assert.h>
#include <pci/express.h>
#include <mm/virtual.h>
#include <util/list.h>
#include <stdio.h>
#include <vy.h>

static list_t pcie_segments;

void pci_express_init(void) {
	acpi_mcfg_t *mcfg;
	acpi_status status = acpi_get_table((char *) "MCFG", 0, (struct acpi_table_header **) &mcfg);
	assert(status == AE_OK && mcfg);

	pcie_config_t *entries = (pcie_config_t *) ((uintptr_t) mcfg + sizeof(acpi_mcfg_t));

	for(size_t i = 0; i < (mcfg->length - 44) >> 4; i++) {
	#ifdef CONFIG_PCIE_DEBUG
		printf("entry %zu: segment %hu, buses %hhu - %hhu, base %#018lx\n", i, entries[i].seg, entries[i].bus_start, entries[i].bus_end, entries[i].addr);
	#endif
		node_t *node = (node_t *) malloc(sizeof(node_t));

		pcie_segment_t *segment = (pcie_segment_t *) malloc(sizeof(pcie_segment_t));
		segment->seg = entries[i].seg;
		segment->bus_start = entries[i].bus_start;
		segment->bus_end = entries[i].bus_end;
		segment->phys = entries[i].addr;

		node->data = segment;

		list_append(&pcie_segments, node);

	}
}

uint64_t pci_express_read(uint16_t segment, uint8_t bus, uint8_t slot, uint8_t function, uint16_t offset) {
	pcie_segment_t *config = NULL;
	node_t *node = pcie_segments.head;

	for(size_t i = 0; i < pcie_segments.length && node; i++, node = node->next) {
		if(((pcie_segment_t *) node->data)->seg == segment) {
			config = node->data;
			break;
		}
	}

	if(!config) {
		panic("[PCIe]	segment %hu not found\n", segment);
	}

	uintptr_t page = config->phys + (uintptr_t) (bus << 20 | slot << 15 | function << 12);
	uintptr_t virt = mm_virtual_alloc(1);
	mm_virtual_map(page, virt, 1, PAGE_PRESENT | PAGE_WRITE);

	assert(offset <= (0x1000 - 8));

	/* unaligned memory access is UB, regardless of whether the arch allows it */
	uint8_t *mmio = (uint8_t *) virt;
	uint64_t data = ((uint64_t) mmio[offset+0] << 0U) | ((uint64_t) mmio[offset+1] << 8U) | ((uint64_t) mmio[offset+2] << 16U) | ((uint64_t) mmio[offset+3] << 24U) | ((uint64_t) mmio[offset+4] << 32U) | ((uint64_t) mmio[offset+5] << 40U) | ((uint64_t) mmio[offset+6] << 48U) | ((uint64_t) mmio[offset+7] << 56U);

	mm_virtual_map(page, virt, 1, 0);
	mm_virtual_free(virt);

	return data;
}