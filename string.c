#include "string.h"

void *memcpy(void *a, void *b, size_t s) {
	char *ca = a;
	char *cb = b;
	ptrdiff_t i;
	for (i = 0; i < s; i++)
		ca[i] = cb[i];
	return a;
}
