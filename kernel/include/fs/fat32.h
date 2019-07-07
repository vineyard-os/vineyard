#pragma once

#include <driver/nvme.h>
#include <stdbool.h>
#include <stdint.h>

bool fat32_check_partition(nvme_ns_t *ns, uint64_t start, size_t len);