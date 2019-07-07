#pragma once

#include <driver/nvme.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

bool btrfs_check_partition(nvme_ns_t *controller, uint64_t start, size_t len);