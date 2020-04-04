#pragma once

#include <stddef.h>

typedef struct node_t {
	struct node_t *prev;
	struct node_t *next;

	void *data;
} node_t;

typedef struct {
	node_t *head;
	node_t *tail;
	size_t length;
} list_t;

#define list_append_data(list, dat) { \
	node_t *node = malloc(sizeof(node_t)); \
	node->data = (dat); \
	list_append((list), node); \
}

void list_insert_after(list_t *list, node_t *after, node_t *node);
void list_append(list_t *list, node_t *node);
void list_remove(list_t *list, node_t *node);
