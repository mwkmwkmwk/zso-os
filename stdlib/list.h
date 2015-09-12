#pragma once

struct list {
	struct list* prev;
	struct list* next;
};

void list_init(struct list* list_head);
void list_insert_after(struct list* where, struct list* new_elem);
void list_insert_before(struct list* where, struct list* new_elem);
void list_unlink(struct list* elem);
