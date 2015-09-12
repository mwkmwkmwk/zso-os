#include "list.h"

static inline void fix_neighbours(struct list* elem) {
	elem->prev->next = elem;
	elem->next->prev = elem;
}

void list_init(struct list* list_head) {
	list_head->prev = list_head;
	list_head->next = list_head;
}

void list_insert_after(struct list* where, struct list* new_elem) {
	new_elem->next = where->next;
	new_elem->prev = where;
	fix_neighbours(new_elem);
}

void list_insert_before(struct list* where, struct list* new_elem) {
	new_elem->next = where;
	new_elem->prev = where->prev;
	fix_neighbours(new_elem);
}

void list_unlink(struct list* elem) {
	elem->prev->next = elem->next;
	elem->next->prev = elem->prev;
}

