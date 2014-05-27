#include <stdarg.h>
#include <stdint.h>
#include "print.h"
#include "vga.h"

#define ROWS 25
#define COLS 80

#define SCREEN_CHAR(ROW, COL) *(uint16_t *)(0xb8000 + sizeof(uint16_t) * ((COL) + COLS * (ROW)))

#define TAB_SIZE 8

#define NORMAL_COLOR 0x0700
#define LIGHT_COLOR 0x0f00

static uint8_t cur_row = 0;
static uint8_t cur_col = 0;

static void fix_vga_cursor(void) {
	vga_set_cursor_addr(cur_col + COLS * cur_row);
}

void init_screen(void) {
	uint16_t pos;
	
	pos = vga_get_cursor_addr() & 0xff;

	cur_row = pos / COLS;
	cur_col = pos % COLS;

	new_line();
}

void cls(void) {
	int row, col;

	for (row = 0; row < ROWS; ++row) {
		for (col = 0; col < COLS; ++col) {
			SCREEN_CHAR(row, col) = NORMAL_COLOR | ' ';
		}
	}

	cur_row = 0;
	cur_col = 0;
	
	fix_vga_cursor();
}

void new_line(void) {
	int row, col;

	// shifting line
	if (cur_row == ROWS - 1) {
		for (row = 0; row < ROWS - 1; ++row) {
			for (col = 0; col < COLS; ++col) {
				SCREEN_CHAR(row, col) = SCREEN_CHAR(row + 1, col);
			}
		}
	} else {
		++cur_row;
	}

	for (col = 0; col < COLS; ++col) {
		SCREEN_CHAR(cur_row, col) = NORMAL_COLOR | ' ';
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
			SCREEN_CHAR(cur_row, cur_col) = NORMAL_COLOR | ch;
			++cur_col;

			if (cur_col == COLS) {
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

static void print_digit(int digit) {
	if (digit < 10) {
		put('0' + digit);
	} else {
		put('a' + digit - 10);
	}
}

static void print_int(int x, int base) {
	int printed;
	int pow;

	if (x < 0) {
		// TODO modify to properly display MIN_INT
		put('-');
		x *= -1;
	}

	pow = 1;

	while (x >= pow * base) {
		pow *= base;
	}

	while (pow != 0) {
		print_digit(x / pow);
		x -= (x / pow) * pow;
		pow /= base;
	}
}

void printf(const char *format, ...) {
	va_list ap;
	int i;

	va_start(ap, format);

	i = 0;
	while (format[i] != '\0') {
		if (format[i] == '%') {
			++i;

			if (format[i] == '\0') {
				put('%');
				break;
			} else switch (format[i]) {
				case '%':
					put('%');
					break;

				case 's':
					print(va_arg(ap, const char *));
					break;

				case 'd':
					print_int(va_arg(ap, int), 10);
					break;

				case 'x':
					print_int(va_arg(ap, int), 16);
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

	va_end(ap);
}
