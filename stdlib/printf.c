#include <stdarg.h>

#include "string.h"
#include "video/vga.h"

#define TAB_SIZE 4

void putc(char ch) {
	const int CONSOLE_WIDTH = 80;
	int cur_pos = get_cursor();
	struct vga_char *cur = (void *)((char *)framebuffer + cur_pos);
	
	if (ch == '\n') {
		cur_pos += CONSOLE_WIDTH*2 - cur_pos % (CONSOLE_WIDTH*2);
	} else if (ch == '\b') {
		if (cur_pos % (CONSOLE_WIDTH*2) > 0)
			cur_pos -= 2;
		(cur-1)->ch = ' ';
		(cur-1)->attr = 0x0a;
	} else if (ch == '\t') {
		int pos = (cur_pos % (CONSOLE_WIDTH*2))/2;
		int spaces = TAB_SIZE - pos % TAB_SIZE;
		while (spaces--)
			putc(' ');
		cur_pos = get_cursor();
	} else {
		cur->ch = ch;
		cur->attr = 0x0a;
		cur_pos += 2;
	}
	if (cur_pos == CONSOLE_WIDTH * 2 * 25) {
		memcpy((void *)0xb8000, (void *)(0xb8000 + 160), 24 * 80 * 2);
		cur_pos -= 160;
		memset((void *)(0xb8000 + 24 * 80 * 2), 0, 160);
	}
	set_cursor(cur_pos);
}

void puts(const char* str) {
	while (*str)
		putc(*str++);
}

void uint32_to_str(char* dst, uint32_t val, uint32_t base) {
	char* org = dst;
	int digit;
	do {
		digit = (val % base);
		if (digit < 10)
			*dst++ = digit + '0';
		else
			*dst++ = digit + 'a' - 10;
		val /= base;
	} while (val > 0);
	*dst++ = '\0';
	strrev(org);
}

void printf(const char *fmt, ...) {
	
	va_list ap;
	char ch;
	char int_str[32];

	va_start(ap, fmt);
	
	int i = 0;
	while (fmt[i]) {
		ch = fmt[i++];
		if (ch != '%') {
			putc(ch);
			continue;
		}

		// Parse format specifier
		ch = fmt[i++];
		if (!ch)
			break;

		switch (ch) {
			case '%':
				putc('%');
				break;
			case 'x':
				uint32_to_str(int_str, va_arg(ap, unsigned int), 16);
				puts(int_str);
				break;
			case 'u':
				uint32_to_str(int_str, va_arg(ap, unsigned int), 10);
				puts(int_str);
				break;
			case 'c':
				putc(va_arg(ap, unsigned int));
				break;
			case 's':
				puts(va_arg(ap, const char*));
				break;
			default:
				puts("\nERROR in printf(): format specifier not supported!\n");
		}
	}

	va_end(ap);
}
