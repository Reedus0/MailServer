#pragma once
#include <stddef.h>

#define list_parent(ptr, parent_struct, list_name) ((char*)ptr - offsetof(parent_struct, list_name))

struct list {
	struct list* prev;
	struct list* next;
};

struct list init_list();
enum STATUS list_insert(struct list* base_list, struct list* new_list);
enum STATUS list_remove(struct list* removed_list);