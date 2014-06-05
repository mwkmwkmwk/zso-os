#include <stdint.h>
#include "print.h"

static void main_debug() {
	// basic print module testing on default window
	if (0) {
		cls(default_window);

		putc(default_window, 't');
		putc(default_window, 'a');
		putc(default_window, 'b');
		putc(default_window, 's');
		putc(default_window, '\n');
		puts(default_window, "12\t2\t232\taax\n211\t231\t2135\t21111\n");
		printf(default_window, "percent: %% incorrect: %! strings: %s %s\n", "string1", "string2");
		printf(default_window, "257 %%d: %d %%x: %x\n", 257, 257);
		printf(default_window, "-257 %%d: %d %%x: %x\n", -257, -257);
		printf(default_window, "0 %%d: %d %%x: %x\n", 0, 0);

		printf(default_window, "INT32_MAX %d INT32_MIN %d\n", INT32_MAX, INT32_MIN);
		printf(default_window, "UINT32_MAX %u %x %p\n", UINT32_MAX, UINT32_MAX, (void *)UINT32_MAX);
		printf(default_window, "UINT32_MIN=0 %u %x %p\n", 0, 0, (void *)0);
	}

	// testing windows
	if (0) {
		fill(default_window, '_');

		set_cursor_pos(default_window, MK_WIN_POINT(0, 0));
		puts(default_window, "WINDOWS TEST");

		{
			INIT_WINDOW(struct window win);
			win.top_left = MK_WIN_POINT(2, 1);
			win.bottom_right = MK_WIN_POINT(17, 3);

			set_color(&win, NORMAL_COLOR);
			set_scroll_mode(&win, REPEAT_MODE);
			printf(&win, "XXXXXXXXXXXXX\n\n\n");
			printf(&win, "################");
			printf(&win, "#              #");
			printf(&win, "################");
			set_cursor_pos(&win, MK_WIN_POINT(2, 1));
			set_color(&win, WHITE_COLOR);
			printf(&win, "%d x %d", WIN_SIZE(&win).width,
					WIN_SIZE(&win).height);
		}

		set_active_window(0);

		{
			int i;

			INIT_WINDOW(struct window win);
			win.top_left = MK_WIN_POINT(20, 0);
			win.bottom_right = MK_WIN_POINT(27, 5);

			set_scroll_mode(&win, REPEAT_MODE);

			for (i = 0; i < 16; ++i) {
				set_color(&win, i);
				putc(&win, '#');
			}

			for (i = 0; i < 16; ++i) {
				set_color(&win, i << 4);
				putc(&win, '#');
			}
		}

		set_active_window(default_window);
		set_cursor_pos(default_window, MK_WIN_POINT(0, 20));
	}
}
