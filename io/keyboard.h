#pragma once
// Kernel/User header

#include <stdbool.h>

enum KEY_CODE {
	KEY_ESCAPE,
	KEY_1,
	KEY_2,
	KEY_3,
	KEY_4,
	KEY_5,
	KEY_6,
	KEY_7,
	KEY_8,
	KEY_9,
	KEY_0,
	KEY_MINUS,
	KEY_EQUAL,
	KEY_BACKSPACE,
	KEY_TAB,
	KEY_Q,
	KEY_W,
	KEY_E,
	KEY_R,
	KEY_T,
	KEY_Y,
	KEY_U,
	KEY_I,
	KEY_O,
	KEY_P,
	KEY_LBRACKET,
	KEY_RBRACKET,
	KEY_ENTER,
	KEY_LEFTCONTROL,
	KEY_A,
	KEY_S,
	KEY_D,
	KEY_F,
	KEY_G,
	KEY_H,
	KEY_J,
	KEY_K,
	KEY_L,
	KEY_SEMICOLON,
	KEY_SINGLEQUOTE,
	KEY_BACKTICK,
	KEY_LSHIFT,
	KEY_BACKSLASH,
	KEY_Z,
	KEY_X,
	KEY_C,
	KEY_V,
	KEY_B,
	KEY_N,
	KEY_M,
	KEY_COMMA,
	KEY_DOT,
	KEY_SLASH,
	KEY_RSHIFT,
	KEY_NUM_STAR,
	KEY_LEFTALT,
	KEY_SPACE,
	KEY_CAPSLOCK,
	KEY_F1,
	KEY_F2,
	KEY_F3,
	KEY_F4,
	KEY_F5,
	KEY_F6,
	KEY_F7,
	KEY_F8,
	KEY_F9,
	KEY_F10,
	KEY_NUMBERLOCK,
	KEY_SCROLLLOCK,
	KEY_NUM_7,
	KEY_NUM_8,
	KEY_NUM_9,
	KEY_NUM_MINUS,
	KEY_NUM_4,
	KEY_NUM_5,
	KEY_NUM_6,
	KEY_NUM_PLUS,
	KEY_NUM_1,
	KEY_NUM_2,
	KEY_NUM_3,
	KEY_NUM_0,
	KEY_NUM_DOT,
	KEY_F11,
	KEY_F12,
	KEY_NUM_ENTER,
	KEY_RIGHTCONTROL,
	KEY_NUM_SLASH,
	KEY_RIGHTALT,
	KEY_HOME,
	KEY_CURSORUP,
	KEY_PAGEUP,
	KEY_CURSORLEFT,
	KEY_CURSORRIGHT,
	KEY_END,
	KEY_CURSORDOWN,
	KEY_PAGEDOWN,
	KEY_INSERT,
	KEY_DELETE,
	KEY_LEFTGUI,
	KEY_RIGHTGUI,
	KEY_APPS,
	KEY_PRINTSCREEN,
	KEY_PAUSE,
};

struct keyboard_event {
	enum KEY_CODE key_code;
	char ascii; // or null if not applicable
	bool pressed;
	bool released;
};

#define KEYBOARD_BUF_SIZE 512

// TODO: make general cyclic struct from this
struct keyboard_buffer {
	struct keyboard_event buffer[KEYBOARD_BUF_SIZE];
	int start; // inclusive
	int end; // exclusive
	// start == end: empty queue
};

#ifndef USERMODE
void init_keyboard(void);
void init_keyboard_buffer(struct keyboard_buffer* buffer);
struct keyboard_buffer* get_active_key_buffer(void);
void set_active_key_buffer(struct keyboard_buffer* buffer);
bool pull_key_event(struct keyboard_buffer* buffer, struct keyboard_event* out); // true on success
#endif
