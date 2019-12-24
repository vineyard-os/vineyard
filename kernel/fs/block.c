#include <fs/block.h>
#include <stdlib.h>
#include <util/list.h>

static list_t block_list;

void block_add(block_t *dev) {
	node_t *node = malloc(sizeof(node_t));
	node->data = dev;

	list_append(&block_list, node);
}