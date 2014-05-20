#include <stdint.h>
#include "vga.h"

#define ROWS 25
#define COLS 80

#define SCREEN_CHAR(ROW, COL) *(uint16_t *)(0xb8000 + sizeof(uint16_t) * ((COL) + COLS * (ROW)))

#define NORMAL_COLOR 0x0700
#define LIGHT_COLOR 0x0f00

static uint8_t cur_row = 0;
static uint8_t cur_col = 0;

static void fix_vga_cursor(void) {
	vga_set_cursor_addr(cur_col + COLS * cur_row);
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
	SCREEN_CHAR(cur_row, cur_col) = NORMAL_COLOR | ch;

	++cur_col;

	if (cur_col == COLS) {
		new_line();
	}

	fix_vga_cursor();
}

void print(const char *msg) {
	while (*msg != '\0') {
		// TODO \r, \t and stuff
		if (*msg == '\n') {
			new_line();
		} else {
			put(*msg);
		}

		++msg;
	}
}
