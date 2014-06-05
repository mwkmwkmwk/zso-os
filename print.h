#ifndef PRINT_H
#define PRINT_H

#include <stdint.h>

#define SCREEN_ROWS 25
#define SCREEN_COLS 80

#define TAB_SIZE 8

#define BLACK_COLOR 0x00
#define BLUE_COLOR 0x01
#define GREEN_COLOR 0x02
#define AQUA_COLOR 0x03
#define RED_COLOR 0x04
#define PURPLE_COLOR 0x05
#define ORANGE_COLOR 0x06
#define LIGHT_GRAY_COLOR 0x07
#define DARK_GRAY_COLOR 0x08
#define LIGHT_BLUE_COLOR 0x09
#define LIGHT_GREEN_COLOR 0x0a
#define LIGHT_AQUA_COLOR 0x0b
#define LIGHT_RED_COLOR 0x0c
#define PINK_COLOR 0x0d
#define YELLOW_COLOR 0x0e
#define WHITE_COLOR 0x0f

#define BLACK_BACKGROUND 0x00
#define BLUE_BACKGROUND 0x10
#define GREEN_BACKGROUND 0x20
#define AQUA_BACKGROUND 0x30
#define RED_BACKGROUND 0x40
#define PURPLE_BACKGROUND 0x50
#define ORANGE_BACKGROUND 0x60
#define LIGHT_GRAY_BACKGROUND 0x70
#define DARK_GRAY_BACKGROUND 0x80
#define LIGHT_BLUE_BACKGROUND 0x90
#define LIGHT_GREEN_BACKGROUND 0xa0
#define LIGHT_AQUA_BACKGROUND 0xb0
#define LIGHT_RED_BACKGROUND 0xc0
#define PINK_BACKGROUND 0xd0
#define YELLOW_BACKGROUND 0xe0
#define WHITE_BACKGROUND 0xf0

#define NORMAL_COLOR LIGHT_GRAY_COLOR

#define SHIFT_MODE 0
#define REPEAT_MODE 1

// top-left corner is (0,0)
// x increases towards right
// y increases towards bottom
struct win_point {
	int x;
	int y;
};

struct win_size {
	int width;
	int height;
};

// structure can be safely read, but modifying it manually is not recommended,
// as some changes might be lost or appear later
struct window {
	struct win_point top_left;
	struct win_point bottom_right;
	struct win_point cursor_pos;
	uint8_t color;
	uint8_t scroll_mode;
};

// used to initialize window to default values
// examples:
// int foo(void) { INIT_WINDOW(struct window win); /* ... */ }
// (struct str_with_win) { INIT_WINDOW(.win), .field = 0 }
#define INIT_WINDOW(WIN) \
	WIN = { \
		.top_left = { \
			.x = 0, \
			.y = 0 \
		}, \
		.bottom_right = { \
			.x = SCREEN_COLS - 1, \
			.y = SCREEN_ROWS - 1\
		}, \
		.cursor_pos = { \
			.x = 0, \
			.y = 0 \
		}, \
		.color = NORMAL_COLOR, \
		.scroll_mode = SHIFT_MODE \
	}

#define MK_WIN_POINT(X, Y) ((struct win_point) { .x = (X), .y = (Y) })

#define WIN_SIZE(WIN) \
	((struct win_size) { \
		.width = (WIN)->bottom_right.x - (WIN)->top_left.x + 1, \
		.height = (WIN)->bottom_right.y - (WIN)->top_left.y + 1 \
	})

// currently active window, i.e. that which holds cursor,
// if equal to 0, cursor is not displayed
extern struct window *active_window;

// holds default window that takes up whole screen, SHIFT_MODE by default
extern struct window *default_window;

// initializes default window without wiping it, reads current cursor position
extern void init_screen(void);

// fills window with specified character
extern void fill(struct window *win, char ch);

// clears screen, sets cursor to top-left corner, resets color
extern void cls(struct window *win);

// starts new line, shifts text towards top if screen full or goes back to first line,
// depending on scroll_mode
extern void new_line(struct window *win);

// puts one character where cursor was and moves it forward, possibly generating new line
// supports special characters:
// \n - new line
// \r - set cursor at the beginning of current line without wiping it
// \t - print 1-8 spaces until cursor column is divisible by TAB_SIZE
extern void putc(struct window *win, char ch);

// prints a string by putting sequence of characters until (not including) '\0'
extern void puts(struct window *win, const char *msg);

// printf supporting %d (int), %x (int), %s (const char *)
extern void printf(struct window *win, const char *format, ...);

// changes active window (and cursor position with it)
extern void set_active_window(struct window *win);

// sets color of written characters, by default it is light gray on black background
// color is bitwise OR of *_COLOR for foreground and *_BACKGROUND for background
extern void set_color(struct window *win, uint8_t color);

// sets cursor position in a window, relative to window top-left corner (0,0)
extern void set_cursor_pos(struct window *win, struct win_point pos);

// sets scrolling mode, i.e. behavior of new_line on last line in the window:
// SHIFT_MODE - discard first line, move everything 1 line up, wipe next
//              line with current background color and set cursor at its beginning.
//              Useful for writing large amounts of text (i.e. console).
// REPEAT_MODE - set cursor back in (0,0) without wiping anything. Useful for
//               ascii-art graphics.
extern void set_scroll_mode(struct window *win, uint8_t mode);

#endif
