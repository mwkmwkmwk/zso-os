#include <stdarg.h>
#include <stdint.h>
#include "print.h"
#include "stdlib.h"
#include "vga.h"

#define NORMAL_COLOR 0x0700
#define LIGHT_COLOR 0x0f00

struct screen {
	uint16_t buf[SCREEN_ROWS][SCREEN_COLS];
};

static struct screen *screen = (struct screen *)0xb8000;

static uint8_t cur_row = 0;
static uint8_t cur_col = 0;

static void fix_vga_cursor(void) {
	vga_set_cursor_addr(cur_col + SCREEN_COLS * cur_row);
}

void init_screen(void) {
	uint16_t pos;
	
	pos = vga_get_cursor_addr();

	cur_row = pos / SCREEN_COLS;
	cur_col = pos % SCREEN_COLS;

	new_line();
}

void cls(void) {
	int row, col;

	for (row = 0; row < SCREEN_ROWS; ++row) {
		for (col = 0; col < SCREEN_COLS; ++col) {
			screen->buf[row][col] = NORMAL_COLOR | ' ';
		}
	}

	cur_row = 0;
	cur_col = 0;
	
	fix_vga_cursor();
}

void new_line(void) {
	int row, col;

	// shifting line
	if (cur_row == SCREEN_ROWS - 1) {
		for (row = 0; row < SCREEN_ROWS - 1; ++row) {
			for (col = 0; col < SCREEN_COLS; ++col) {
				screen->buf[row][col] = screen->buf[row + 1][col];
			}
		}
	} else {
		++cur_row;
	}

	for (col = 0; col < SCREEN_COLS; ++col) {
		screen->buf[cur_row][col] = NORMAL_COLOR | ' ';
	}

	cur_col = 0;
	fix_vga_cursor();
}

void put(char ch) {
	// TODO additional special characters
	switch (ch) {
		case '\n':
			new_line();
			break;

		case '\r':
			cur_col = 0;
			break;

		case '\t':
			do {
				put(' ');
			} while (cur_col % TAB_SIZE != 0);
			break;

		default:
			screen->buf[cur_row][cur_col] = NORMAL_COLOR | ch;
			++cur_col;

			if (cur_col == SCREEN_COLS) {
				new_line();
			}

			break;
	}

	fix_vga_cursor();
}

void print(const char *msg) {
	while (*msg != '\0') {
		put(*msg);
		++msg;
	}
}

// works up to base 36
static char digit2char[] = "0123456789abcdefghijklmnopqrstuvwxyz";

// it's user responsibility that width <= buf_size - 1
char *sprint_uint(char *buf, int buf_size, uint32_t x, int base, int width, char fill) {
	int i = buf_size - 1;
	buf[i--] = '\0';

	do {
		buf[i--] = digit2char[x % base];
		x /= base;
	} while (x > 0);

	while (i + width + 2 > buf_size) {
		buf[i--] = fill;
	}

	return buf + i + 1;
}

char *sprint_int(char *buf, int buf_size, int32_t x, int base, int width, char fill) {
	char *res;

	res = sprint_uint(buf + 1, buf_size - 1, abs_i32_to_u32(x),
			base, width == 0 ? 0 : width - 1, fill);

	if (x < 0) {
		*(--res) = '-';
		return res;
	} else {
		return res;
	}
}

#define INT_BUF_SIZE 12

void printf(const char *format, ...) {
	va_list ap;
	int i;
	char buf[INT_BUF_SIZE];

	va_start(ap, format);

	i = 0;
	while (format[i] != '\0') {
		if (format[i] == '%') {
			++i;

			switch (format[i]) {
				case '\0':
					put('%');
					goto finish;

				case '%':
					put('%');
					break;

				case 's':
					print(va_arg(ap, const char *));
					break;

				case 'd':
					print(sprint_int(buf, INT_BUF_SIZE, va_arg(ap, int32_t), 10, 0, 0));
					break;

				case 'u':
					print(sprint_uint(buf, INT_BUF_SIZE, va_arg(ap, uint32_t), 10, 0, 0));
					break;

				case 'x':
					print(sprint_uint(buf, INT_BUF_SIZE, va_arg(ap, uint32_t), 16, 0, 0));
					break;

				case 'p':
					print("0x");
					print(sprint_uint(buf, INT_BUF_SIZE, (uint32_t)va_arg(ap, void *), 16, 8, '0'));
					break;

				default:
					// simply print incorrect options
					put('%');
					put(format[i]);
					break;
			}
		} else {
			put(format[i]);
		}

		++i;
	}

finish:
	va_end(ap);
}
