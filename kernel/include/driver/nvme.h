#pragma once

#include <stddef.h>
#include <stdint.h>

#define barrier() asm volatile ("": : :"memory")

/* doorbell stride */
#define NVME_CAP_DSTRD(x) ((x >> 32) & 0x0F)
/* CSTS.RDY timeout */
#define NVME_CAP_TO(x) ((x >> 24) & 0xFF)
#define NVME_CAP_CSS_NVME (1UL << 37)

struct nvme;

/* controller registers - 3.1 */
typedef struct {
    uint64_t cap;
    uint32_t vs;
    uint32_t intms;
    uint32_t intmc;
    uint32_t cc;
    uint32_t reserved0;
    uint32_t csts;
    uint32_t nssr;
    uint32_t aqa;
    uint64_t asq;
    uint64_t acq;
    uint32_t cmbloc;
    uint32_t cmbsz;
    uint32_t bpinfo;
    uint32_t bprsel;
    uint64_t bpmbl;
} __attribute__((packed)) nvme_reg_t;

/* completion queue entry - 4.6 */
typedef struct nvme_cq_entry {
	union {
		uint32_t dword[4];
		struct {
			uint32_t cmd;
			uint32_t reserved;
			uint16_t sq_head;
			uint16_t sq_id;
			uint16_t command_id;
			uint16_t status;
		};
	};
} __attribute__((packed)) nvme_cq_entry_t;

typedef struct {
	volatile uint32_t *doorbell;
	uint32_t mask;
	nvme_cq_entry_t *entry;

	uint32_t head;
	uint8_t phase;
} nvme_cq_t;

/* submission queue entry - 4.2 */
typedef struct {
	union {
		uint32_t dword[16];
		struct {
			uint32_t cmd;
			uint32_t ns_id;
			uint64_t reserved;
			uint64_t metadata_ptr;
			uint64_t data_prp1;
			uint64_t data_prp2;
		};
	};
} __attribute__((packed)) nvme_sq_entry_t;

typedef struct {
	volatile uint32_t *doorbell;
	uint32_t mask;
	nvme_sq_entry_t *entry;

	nvme_cq_t *cq;
	uint32_t head;
	uint32_t tail;
} nvme_sq_t;

typedef struct {
	struct nvme *controller;
	uint32_t id;
	uint64_t lba_count;
	uint32_t block_size;
	uint32_t metadata_size;
} nvme_ns_t;

typedef struct nvme {
	uintptr_t base;
	nvme_reg_t volatile *reg;
	uint32_t doorbell_stride;

	nvme_cq_t admin_cq;
	nvme_sq_t admin_sq;

	nvme_ns_t *namespace_list;
	size_t namespaces;

	size_t id;
	char *serial;
	char *model;
	char *firmware;

	nvme_cq_t io_cq;
	nvme_sq_t io_sq;

} nvme_t;

/* Figure 111, p. 129 of version 1.3d */
typedef struct {
	uint16_t metadata_size;
	uint8_t lba_data_size;
	uint8_t relative_performance;
	uint8_t reserved;
} nvme_lba_format_t;

typedef struct {
	uint16_t pci_vendor;
	uint16_t pci_subsystem;
	char serial[20];
	char model[40];
	char firmware[8];
	uint8_t rab;
	uint8_t ieee_oui[3];
	uint8_t cmic;
	uint8_t max_data_transfer;
	uint16_t controller_id;
	uint32_t version;
	uint32_t rtd3r;
	uint32_t rtd3e;
	uint32_t oaes;
	uint32_t controller_attr;
	uint8_t reserved0[12];
	uint8_t fru_guid[16];
	uint8_t reserved1[388];
	uint32_t namespaces;
} __attribute__((packed)) nvme_id_controller_t;

/* Figure 110, p. 122 of version 1.3d */
typedef struct {
	uint64_t ns_size;
	uint64_t ns_capacity;
	uint64_t ns_utilization;
	uint8_t ns_features;
	uint8_t lba_formats;
	uint8_t formatted_lba_size;
	uint8_t bytes[101];
	nvme_lba_format_t lba_format[16];
} __attribute__((packed)) nvme_id_ns_t;

void nvme_wait_csts_rdy(nvme_t *controller, uint8_t rdy);
nvme_cq_entry_t *nvme_wait(nvme_sq_t *sq);
int nvme_poll_cq(nvme_cq_t *cq);

void nvme_cq_init(nvme_t *controller, nvme_cq_t *cq, uint16_t index, uint16_t length);
void nvme_sq_init(nvme_t *controller, nvme_sq_t *sq, uint16_t index, uint16_t length, nvme_cq_t *cq);
void nvme_io_cq_init(nvme_t *controller, nvme_cq_t *cq, uint16_t index);
void nvme_io_sq_init(nvme_t *controller, nvme_sq_t *sq, uint16_t index, nvme_cq_t *cq);

nvme_id_controller_t *nvme_identify_controller(nvme_t *controller);
nvme_id_ns_t *nvme_identify_ns(nvme_t *controller, uint8_t id);

nvme_sq_entry_t *nvme_command_create(nvme_sq_t *sq, uint8_t opcode, uintptr_t metadata, uintptr_t data);
void nvme_command_submit(nvme_sq_t *sq);
nvme_cq_entry_t *nvme_command_cqe(nvme_sq_t *sq);

nvme_t *nvme_init(uint8_t bus, uint8_t slot, uint8_t function);

void nvme_rw_read(nvme_ns_t *ns, uint64_t lba, char *buf);
void nvme_read(nvme_ns_t *ns, uint64_t physical, size_t length, char *buf);

void nvme_ns_probe(nvme_t *controller, uint8_t ns);
