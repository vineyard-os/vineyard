#pragma once

#include <fs/common.h>

typedef struct {
	char *name;
} block_t;

void block_add(block_t *dev);