#include <stdarg.h>
#include <stdint.h>
#include "print.h"
#include "string.h"
#include "vga.h"

struct screen {
	uint16_t buf[SCREEN_ROWS][SCREEN_COLS];
};

static struct screen *screen = (struct screen *)0xb8000;

struct window *active_window = 0;

INIT_WINDOW(static struct window __default_window);

struct window *default_window = &__default_window;

static uint16_t colored_char(struct window *win, char ch) {
	return ((uint16_t)win->color << 8) | ch;
}

static void fix_vga_cursor(void) {
	if (active_window != 0) {
		vga_set_cursor_addr(active_window->top_left.x +
				active_window->cursor_pos.x +
				SCREEN_COLS * (active_window->top_left.y +
					active_window->cursor_pos.y));
	} else {
		// setting cursor outside of visible area makes it disappear
		vga_set_cursor_addr(SCREEN_COLS * SCREEN_ROWS);
	}
}

void init_screen(void) {
	uint16_t pos;
	
	pos = vga_get_cursor_addr();

	default_window->cursor_pos = (struct win_point) {
		.x = pos % SCREEN_COLS,
		.y = pos / SCREEN_COLS
	};

	new_line(default_window);

	set_active_window(default_window);
}

void fill(struct window *win, char ch) {
	int row, col;

	for (row = win->top_left.y; row <= win->bottom_right.y; ++row) {
		for (col = win->top_left.x; col <= win->bottom_right.x; ++col) {
			screen->buf[row][col] = colored_char(win, ch);
		}
	}

	if (win == active_window) {
		fix_vga_cursor();
	}
}

void cls(struct window *win) {
	set_color(win, NORMAL_COLOR);
	win->cursor_pos = MK_WIN_POINT(0, 0);

	fill(win, ' ');
}

void new_line(struct window *win) {
	int row, col;

	if (win->scroll_mode == SHIFT_MODE) {
		// shifting line
		if (win->cursor_pos.y == WIN_SIZE(win).height - 1) {
			for (row = win->top_left.y; row <= win->bottom_right.y - 1; ++row) {
				for (col = win->top_left.x; col <= win->bottom_right.x; ++col) {
					screen->buf[row][col] = screen->buf[row + 1][col];
				}
			}
		} else {
			++win->cursor_pos.y;
		}

		// wiping line
		for (col = win->top_left.x; col <= win->bottom_right.x; ++col) {
			screen->buf[win->top_left.y + win->cursor_pos.y][col] =
				colored_char(win, ' ');
		}

		win->cursor_pos.x = 0;
	} else { // REPEAT_MODE
		win->cursor_pos.y = (win->cursor_pos.y + 1) % WIN_SIZE(win).height;
		win->cursor_pos.x = 0;
	}

	if (win == active_window) {
		fix_vga_cursor();
	}
}

void putc(struct window *win, char ch) {
	// TODO additional special characters
	switch (ch) {
		case '\n':
			new_line(win);
			break;

		case '\r':
			win->cursor_pos.x = 0;
			break;

		case '\t':
			do {
				putc(win, ' ');
			} while (win->cursor_pos.x % TAB_SIZE != 0);
			break;

		default:
			screen->buf[win->top_left.y + win->cursor_pos.y]
				[win->top_left.x + win->cursor_pos.x] =
				colored_char(win, ch);
			++win->cursor_pos.x;

			if (win->top_left.x + win->cursor_pos.x ==
					win->bottom_right.x + 1) {
				new_line(win);
			}

			break;
	}

	if (win == active_window) {
		fix_vga_cursor();
	}
}

void puts(struct window *win, const char *msg) {
	while (*msg != '\0') {
		putc(win, *msg);
		++msg;
	}
}

#define INT_BUF_SIZE 12

void printf(struct window *win, const char *format, ...) {
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
					putc(win, '%');
					goto finish;

				case '%':
					putc(win, '%');
					break;

				case 's':
					puts(win, va_arg(ap, const char *));
					break;

				case 'd':
					puts(win, sprint_int(buf, INT_BUF_SIZE, va_arg(ap, int32_t), 10, 0, 0));
					break;

				case 'u':
					puts(win, sprint_uint(buf, INT_BUF_SIZE, va_arg(ap, uint32_t), 10, 0, 0));
					break;

				case 'x':
					puts(win, sprint_uint(buf, INT_BUF_SIZE, va_arg(ap, uint32_t), 16, 0, 0));
					break;

				case 'p':
					puts(win, "0x");
					puts(win, sprint_uint(buf, INT_BUF_SIZE, (uint32_t)va_arg(ap, void *), 16, 8, '0'));
					break;

				default:
					// simply print incorrect options
					putc(win, '%');
					putc(win, format[i]);
					break;
			}
		} else {
			putc(win, format[i]);
		}

		++i;
	}

finish:
	va_end(ap);
}

void set_active_window(struct window *win) {
	active_window = win;
	fix_vga_cursor();
}

void set_color(struct window *win, uint8_t color) {
	win->color = color;
}

void set_cursor_pos(struct window *win, struct win_point pos) {
	win->cursor_pos = pos;

	if (win == active_window) {
		fix_vga_cursor();
	}
}

void set_scroll_mode(struct window *win, uint8_t mode) {
	win->scroll_mode = mode;
}
