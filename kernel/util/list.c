#include <util/list.h>

void list_insert_after(list_t *list, node_t *after, node_t *node) {
	if(!after || !node) {
		return;
	}

	node->next = after->next;
	node->prev = after;

	if(!after->next) {
		list->tail = node;
	} else {
		after->next->prev = node;
	}

	after->next = node;

	list->length++;
}

void list_append(list_t *list, node_t *node) {
	if(!node) {
		return;
	}

	if(!list->head) {
		list->head = node;
	}

	if(list->tail) {
		list->tail->next = node;
	}

	node->prev = list->tail;
	list->tail = node;
	list->tail->next = NULL;

	list->length++;
}

void list_remove(list_t *list, node_t *node) {
	if(!node) {
		return;
	}

	if(!node->prev) {
		list->head = node->next;
	} else {
		node->prev->next = node->next;
	}

	if(!node->next) {
		list->tail = node->prev;
	} else {
		node->next->prev = node->prev;
	}

	list->length--;
}
