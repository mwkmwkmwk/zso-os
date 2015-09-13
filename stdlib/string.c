#include "string.h"

void* memcpy(void* dst, const void* src, size_t num) {
	for (int i = 0; i < num ; i++) {
		((char*)dst)[i] = ((char*)src)[i];
	}
	return dst;
}

void* memset(void* dst, int src, size_t num) {
	for (int i = 0; i < num ; i++) {
		((char*)dst)[i] = src;
	}
	return dst;
}

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

char* strncpy(char* dst, const char* src, int dst_size) {
	int i;
	for (i = 0; src[i] && i < dst_size - 1; i++) {
		((char*)dst)[i] = ((char*)src)[i];
	}
	dst[i] = 0;
	return dst;
}
