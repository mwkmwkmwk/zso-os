#include "string.h"

void *memcpy(void *a, const void *b, size_t s) {
	char *ca = a;
	const char *cb = b;
	ptrdiff_t i;
	for (i = 0; i < s; i++)
		ca[i] = cb[i];
	return a;
}

void *memset(void *a, int c, size_t s) {
	char *ca = a;
	ptrdiff_t i;
	for (i = 0; i < s; i++)
		ca[i] = c;
	return a;
}
