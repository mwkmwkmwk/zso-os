#include "string.h"

size_t strlen(const char* str) {
	size_t len = 0;
	while (*str++)
		len++;
	return len;
}

void strrev(char* str) {
	int len = strlen(str);
	int i;
	char tmp;
	for (i = 0; i < len/2; i++) {
		tmp = str[i];
		str[i] = str[len - i - 1];
		str[len - i - 1] = tmp;
	}
}
