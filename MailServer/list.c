#include <stdio.h>
#include "malloc.h"
#include "list.h"
#include "status.h"

struct list init_list() {
	struct list new_list;
	new_list.prev = NULL;
	new_list.next = NULL;

	return new_list;
}

enum STATUS list_insert(struct list* base_list, struct list* new_list) {
	new_list->next = base_list->next;
	base_list->next = new_list;
	new_list->prev = base_list;

	return STATUS_OK;
}

enum STATUS list_remove(struct list* removed_list) {
	removed_list->prev->next = removed_list->next;
	if (removed_list->next != NULL) {
		removed_list->next->prev = removed_list->prev;
	}

	removed_list->prev = NULL;
	removed_list->next = NULL;

	return STATUS_OK;
}