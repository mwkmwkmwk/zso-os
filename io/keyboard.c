#include "keyboard.h"

#include <stdint.h>

#include "stdlib/assert.h"
#include "io/interrupts.h"
#include "io/io.h"
#include "io/pic.h"
#include "mem/gdt.h"
#include "mem/kalloc.h"
#include "panic.h"
#include "stdlib/printf.h"
#include "stdlib/string.h"
#include "threading/context.h"
#include "utils/asm.h"

// Currently assuming that the keyboard uses scan code set 1

bool caps_state;
static volatile struct keyboard_buffer* active_buffer;

// OEM scan code to KEY_CODE translation tree
// TODO: current version wastes a lot of memory, add alternative way to store ptrs for some nodes: list of <key, val>
struct scantree_node {
	bool is_leaf; // a leaf in the tree
	union {
		struct {
			bool pressed;
			bool released; // both can true at the same time (e.g. for pause key)
			enum KEY_CODE key_code;
		} leaf_data;
		struct {
			struct scantree_node* (*next_node)[256]; // pointer to an array of 256 pointers to next nodes (indexed by scan code byte)
		} nonleaf_data;
	};
};

struct key_to_ascii {
	char normal;
	char shifted;
};

bool put_event(struct keyboard_buffer* buffer, enum KEY_CODE key_code, char ascii,
               bool pressed, bool released);

// null-byte cannot be a correct scan code, so we can pass the code as a c-string
void scantree_insert(struct scantree_node* node, const char* scan_code,
                     bool pressed, bool released, enum KEY_CODE key_code) {
	for (int i = 0; scan_code[i]; i++) {
		assert(!node->is_leaf);
		if (node->nonleaf_data.next_node == NULL) {
			// Alloc sub-nodes array
			node->nonleaf_data.next_node = kalloc(sizeof(*node->nonleaf_data.next_node));
			for (int j = 0; j < 256; j++)
				(*(node->nonleaf_data.next_node))[j] = NULL;
		}
		struct scantree_node** next_node_ptr = &((*(node->nonleaf_data.next_node))[(uint8_t)scan_code[i]]);
		if (*next_node_ptr == NULL) {
			*next_node_ptr = kalloc(sizeof(struct scantree_node));
			(*next_node_ptr)->is_leaf = false;
			(*next_node_ptr)->nonleaf_data.next_node = NULL;
		}
		node = *next_node_ptr;
	}
	node->is_leaf = true;
	node->leaf_data.pressed  = pressed;
	node->leaf_data.released = released;
	node->leaf_data.key_code = key_code;
}

struct scantree_node us_qwerty_set1;
struct key_to_ascii us_qwerty_set1_ascii[256];
bool key_state[256]; // true if pressed

static void init_us_qwerty_set1(void);

// Current node in scan codes translation tree
// Invariant: current_state->is_leaf == false
struct scantree_node* current_state;
struct key_to_ascii* current_layout;

static void reset_scan_state(void) {
	current_state = &us_qwerty_set1;
}

static void irq1(struct context** context_ptr) {
	uint8_t code = inb(0x60);
	
	// Doesn't work as it should. Let's skip it.
	// if (code == 0x00
	// 	|| code == 0xAA
	// 	|| code == 0xEE
	// 	|| code == 0xFA
	// 	|| code == 0xFC
	// 	|| code == 0xFD
	// 	|| code == 0xFE
	// 	|| code == 0xFF) {
	// 	// Special codes (e.g. errors)
	// 	// TODO: handle all of them somehow
	// 	reset_scan_state();
	// 	goto RETURN;
	// }

	if (current_state->nonleaf_data.next_node == NULL
		|| (*current_state->nonleaf_data.next_node)[code] == NULL) {
		printf("[Keyboard] Scan sequence not supported!\n");
		reset_scan_state();
		goto RETURN;
	}
	// printf("[Keyboard] Rcvd scan code: %x\n", code);

	current_state = (*current_state->nonleaf_data.next_node)[code];
	if (current_state->is_leaf) {
		bool pressed = current_state->leaf_data.pressed;
		bool released = current_state->leaf_data.released;
		enum KEY_CODE code = current_state->leaf_data.key_code;
		// printf("[Keyboard] Scanned key: %u (%s)\n", code,
		// 	(pressed && released) ? "pressed, released"
		// 	: (pressed ? "pressed" : "released"));
		
		if (pressed)
			key_state[code] = true;
		if (released)
			key_state[code] = false;

		// TODO: caps state should be synchronized somehow else (problem: starting qemu with already enabled capslock)
		if (code == KEY_CAPSLOCK && pressed)
			caps_state ^= 1;

		// Currently weird capslock semantic on non-alpha keys. TODO: fix it
		bool shifted = (key_state[KEY_LSHIFT] || key_state[KEY_RSHIFT]) ^ caps_state;
		char ascii = 0;

		if (pressed) {
			if (!shifted)
				ascii = current_layout[code].normal;
			if (shifted)
				ascii = current_layout[code].shifted;
		}

		if (!put_event(get_active_key_buffer(),
		               code, ascii, pressed, released))
		{
			printf("Keyboard buffer is full or no one has keyboard focus\n");
		}
		reset_scan_state();
	}

RETURN:
	finish_pic_interrupt(INT_KEYBOARD);
}

void init_keyboard(void) {
	register_int_handler(INT_KEYBOARD, irq1, true, 0);
	init_us_qwerty_set1();
	reset_scan_state();
	caps_state = false;
	current_layout = us_qwerty_set1_ascii;
	enable_pic_interrupt(INT_KEYBOARD);
}

void init_keyboard_buffer(struct keyboard_buffer* buffer) {
	bool iflag = disable_interrupts();
	buffer->start = buffer->end = 0;
	set_int_flag(iflag);
}

struct keyboard_buffer* get_active_key_buffer(void) {
	return (struct keyboard_buffer*)atomic_read((uint*)&active_buffer);
}

void set_active_key_buffer(struct keyboard_buffer* buffer) {
	atomic_set((uint*)&active_buffer, (uint)buffer);
}

bool put_event(struct keyboard_buffer* buffer, enum KEY_CODE key_code, char ascii,
               bool pressed, bool released)
{
	if (!buffer)
		return false;
	bool res = false;
	bool iflag = disable_interrupts();
	int new_end = (buffer->end + 1) % _countof(buffer->buffer);
	if (new_end != buffer->start) {
		// TODO: emit 2 separate events if pressed && released (don't confuse user with [pause] weirdness)
		buffer->buffer[buffer->end].key_code = key_code;
		buffer->buffer[buffer->end].ascii    = ascii;
		buffer->buffer[buffer->end].pressed  = pressed;
		buffer->buffer[buffer->end].released = released;
		buffer->end = new_end;
		res = true;
	}
	set_int_flag(iflag);
	return res;
}

bool pull_key_event(struct keyboard_buffer* buffer, struct keyboard_event* out) {
	if (!buffer)
		return false;
	bool res = false;
	bool iflag = disable_interrupts();
	if (buffer->start != buffer->end) {
		*out = buffer->buffer[buffer->start];
		buffer->start = (buffer->start + 1) % _countof(buffer->buffer);
		res = true;
	}
	set_int_flag(iflag);
	return res;
}

static void init_us_qwerty_set1(void) {
	us_qwerty_set1.is_leaf = false;
	us_qwerty_set1.nonleaf_data.next_node = NULL;
	scantree_insert(&us_qwerty_set1, "\x01",	true,	false,	KEY_ESCAPE);
	scantree_insert(&us_qwerty_set1, "\x02",	true,	false,	KEY_1);
	scantree_insert(&us_qwerty_set1, "\x03",	true,	false,	KEY_2);
	scantree_insert(&us_qwerty_set1, "\x04",	true,	false,	KEY_3);
	scantree_insert(&us_qwerty_set1, "\x05",	true,	false,	KEY_4);
	scantree_insert(&us_qwerty_set1, "\x06",	true,	false,	KEY_5);
	scantree_insert(&us_qwerty_set1, "\x07",	true,	false,	KEY_6);
	scantree_insert(&us_qwerty_set1, "\x08",	true,	false,	KEY_7);
	scantree_insert(&us_qwerty_set1, "\x09",	true,	false,	KEY_8);
	scantree_insert(&us_qwerty_set1, "\x0A",	true,	false,	KEY_9);
	scantree_insert(&us_qwerty_set1, "\x0B",	true,	false,	KEY_0);
	scantree_insert(&us_qwerty_set1, "\x0C",	true,	false,	KEY_MINUS);
	scantree_insert(&us_qwerty_set1, "\x0D",	true,	false,	KEY_EQUAL);
	scantree_insert(&us_qwerty_set1, "\x0E",	true,	false,	KEY_BACKSPACE);
	scantree_insert(&us_qwerty_set1, "\x0F",	true,	false,	KEY_TAB);
	scantree_insert(&us_qwerty_set1, "\x10",	true,	false,	KEY_Q);
	scantree_insert(&us_qwerty_set1, "\x11",	true,	false,	KEY_W);
	scantree_insert(&us_qwerty_set1, "\x12",	true,	false,	KEY_E);
	scantree_insert(&us_qwerty_set1, "\x13",	true,	false,	KEY_R);
	scantree_insert(&us_qwerty_set1, "\x14",	true,	false,	KEY_T);
	scantree_insert(&us_qwerty_set1, "\x15",	true,	false,	KEY_Y);
	scantree_insert(&us_qwerty_set1, "\x16",	true,	false,	KEY_U);
	scantree_insert(&us_qwerty_set1, "\x17",	true,	false,	KEY_I);
	scantree_insert(&us_qwerty_set1, "\x18",	true,	false,	KEY_O);
	scantree_insert(&us_qwerty_set1, "\x19",	true,	false,	KEY_P);
	scantree_insert(&us_qwerty_set1, "\x1A",	true,	false,	KEY_LBRACKET);
	scantree_insert(&us_qwerty_set1, "\x1B",	true,	false,	KEY_RBRACKET);
	scantree_insert(&us_qwerty_set1, "\x1C",	true,	false,	KEY_ENTER);
	scantree_insert(&us_qwerty_set1, "\x1D",	true,	false,	KEY_LEFTCONTROL);
	scantree_insert(&us_qwerty_set1, "\x1E",	true,	false,	KEY_A);
	scantree_insert(&us_qwerty_set1, "\x1F",	true,	false,	KEY_S);
	scantree_insert(&us_qwerty_set1, "\x20",	true,	false,	KEY_D);
	scantree_insert(&us_qwerty_set1, "\x21",	true,	false,	KEY_F);
	scantree_insert(&us_qwerty_set1, "\x22",	true,	false,	KEY_G);
	scantree_insert(&us_qwerty_set1, "\x23",	true,	false,	KEY_H);
	scantree_insert(&us_qwerty_set1, "\x24",	true,	false,	KEY_J);
	scantree_insert(&us_qwerty_set1, "\x25",	true,	false,	KEY_K);
	scantree_insert(&us_qwerty_set1, "\x26",	true,	false,	KEY_L);
	scantree_insert(&us_qwerty_set1, "\x27",	true,	false,	KEY_SEMICOLON);
	scantree_insert(&us_qwerty_set1, "\x28",	true,	false,	KEY_SINGLEQUOTE);
	scantree_insert(&us_qwerty_set1, "\x29",	true,	false,	KEY_BACKTICK);
	scantree_insert(&us_qwerty_set1, "\x2A",	true,	false,	KEY_LSHIFT);
	scantree_insert(&us_qwerty_set1, "\x2B",	true,	false,	KEY_BACKSLASH);
	scantree_insert(&us_qwerty_set1, "\x2C",	true,	false,	KEY_Z);
	scantree_insert(&us_qwerty_set1, "\x2D",	true,	false,	KEY_X);
	scantree_insert(&us_qwerty_set1, "\x2E",	true,	false,	KEY_C);
	scantree_insert(&us_qwerty_set1, "\x2F",	true,	false,	KEY_V);
	scantree_insert(&us_qwerty_set1, "\x30",	true,	false,	KEY_B);
	scantree_insert(&us_qwerty_set1, "\x31",	true,	false,	KEY_N);
	scantree_insert(&us_qwerty_set1, "\x32",	true,	false,	KEY_M);
	scantree_insert(&us_qwerty_set1, "\x33",	true,	false,	KEY_COMMA);
	scantree_insert(&us_qwerty_set1, "\x34",	true,	false,	KEY_DOT);
	scantree_insert(&us_qwerty_set1, "\x35",	true,	false,	KEY_SLASH);
	scantree_insert(&us_qwerty_set1, "\x36",	true,	false,	KEY_RSHIFT);
	scantree_insert(&us_qwerty_set1, "\x37",	true,	false,	KEY_NUM_STAR);
	scantree_insert(&us_qwerty_set1, "\x38",	true,	false,	KEY_LEFTALT);
	scantree_insert(&us_qwerty_set1, "\x39",	true,	false,	KEY_SPACE);
	scantree_insert(&us_qwerty_set1, "\x3A",	true,	false,	KEY_CAPSLOCK);
	scantree_insert(&us_qwerty_set1, "\x3B",	true,	false,	KEY_F1);
	scantree_insert(&us_qwerty_set1, "\x3C",	true,	false,	KEY_F2);
	scantree_insert(&us_qwerty_set1, "\x3D",	true,	false,	KEY_F3);
	scantree_insert(&us_qwerty_set1, "\x3E",	true,	false,	KEY_F4);
	scantree_insert(&us_qwerty_set1, "\x3F",	true,	false,	KEY_F5);
	scantree_insert(&us_qwerty_set1, "\x40",	true,	false,	KEY_F6);
	scantree_insert(&us_qwerty_set1, "\x41",	true,	false,	KEY_F7);
	scantree_insert(&us_qwerty_set1, "\x42",	true,	false,	KEY_F8);
	scantree_insert(&us_qwerty_set1, "\x43",	true,	false,	KEY_F9);
	scantree_insert(&us_qwerty_set1, "\x44",	true,	false,	KEY_F10);
	scantree_insert(&us_qwerty_set1, "\x45",	true,	false,	KEY_NUMBERLOCK);
	scantree_insert(&us_qwerty_set1, "\x46",	true,	false,	KEY_SCROLLLOCK);
	scantree_insert(&us_qwerty_set1, "\x47",	true,	false,	KEY_NUM_7);
	scantree_insert(&us_qwerty_set1, "\x48",	true,	false,	KEY_NUM_8);
	scantree_insert(&us_qwerty_set1, "\x49",	true,	false,	KEY_NUM_9);
	scantree_insert(&us_qwerty_set1, "\x4A",	true,	false,	KEY_NUM_MINUS);
	scantree_insert(&us_qwerty_set1, "\x4B",	true,	false,	KEY_NUM_4);
	scantree_insert(&us_qwerty_set1, "\x4C",	true,	false,	KEY_NUM_5);
	scantree_insert(&us_qwerty_set1, "\x4D",	true,	false,	KEY_NUM_6);
	scantree_insert(&us_qwerty_set1, "\x4E",	true,	false,	KEY_NUM_PLUS);
	scantree_insert(&us_qwerty_set1, "\x4F",	true,	false,	KEY_NUM_1);
	scantree_insert(&us_qwerty_set1, "\x50",	true,	false,	KEY_NUM_2);
	scantree_insert(&us_qwerty_set1, "\x51",	true,	false,	KEY_NUM_3);
	scantree_insert(&us_qwerty_set1, "\x52",	true,	false,	KEY_NUM_0);
	scantree_insert(&us_qwerty_set1, "\x53",	true,	false,	KEY_NUM_DOT);
	scantree_insert(&us_qwerty_set1, "\x57",	true,	false,	KEY_F11);
	scantree_insert(&us_qwerty_set1, "\x58",	true,	false,	KEY_F12);
	scantree_insert(&us_qwerty_set1, "\x81",	false,	true,	KEY_ESCAPE);
	scantree_insert(&us_qwerty_set1, "\x82",	false,	true,	KEY_1);
	scantree_insert(&us_qwerty_set1, "\x83",	false,	true,	KEY_2);
	scantree_insert(&us_qwerty_set1, "\x84",	false,	true,	KEY_3);
	scantree_insert(&us_qwerty_set1, "\x85",	false,	true,	KEY_4);
	scantree_insert(&us_qwerty_set1, "\x86",	false,	true,	KEY_5);
	scantree_insert(&us_qwerty_set1, "\x87",	false,	true,	KEY_6);
	scantree_insert(&us_qwerty_set1, "\x88",	false,	true,	KEY_7);
	scantree_insert(&us_qwerty_set1, "\x89",	false,	true,	KEY_8);
	scantree_insert(&us_qwerty_set1, "\x8A",	false,	true,	KEY_9);
	scantree_insert(&us_qwerty_set1, "\x8B",	false,	true,	KEY_0);
	scantree_insert(&us_qwerty_set1, "\x8C",	false,	true,	KEY_MINUS);
	scantree_insert(&us_qwerty_set1, "\x8D",	false,	true,	KEY_EQUAL);
	scantree_insert(&us_qwerty_set1, "\x8E",	false,	true,	KEY_BACKSPACE);
	scantree_insert(&us_qwerty_set1, "\x8F",	false,	true,	KEY_TAB);
	scantree_insert(&us_qwerty_set1, "\x90",	false,	true,	KEY_Q);
	scantree_insert(&us_qwerty_set1, "\x91",	false,	true,	KEY_W);
	scantree_insert(&us_qwerty_set1, "\x92",	false,	true,	KEY_E);
	scantree_insert(&us_qwerty_set1, "\x93",	false,	true,	KEY_R);
	scantree_insert(&us_qwerty_set1, "\x94",	false,	true,	KEY_T);
	scantree_insert(&us_qwerty_set1, "\x95",	false,	true,	KEY_Y);
	scantree_insert(&us_qwerty_set1, "\x96",	false,	true,	KEY_U);
	scantree_insert(&us_qwerty_set1, "\x97",	false,	true,	KEY_I);
	scantree_insert(&us_qwerty_set1, "\x98",	false,	true,	KEY_O);
	scantree_insert(&us_qwerty_set1, "\x99",	false,	true,	KEY_P);
	scantree_insert(&us_qwerty_set1, "\x9A",	false,	true,	KEY_LBRACKET);
	scantree_insert(&us_qwerty_set1, "\x9B",	false,	true,	KEY_RBRACKET);
	scantree_insert(&us_qwerty_set1, "\x9C",	false,	true,	KEY_ENTER);
	scantree_insert(&us_qwerty_set1, "\x9D",	false,	true,	KEY_LEFTCONTROL);
	scantree_insert(&us_qwerty_set1, "\x9E",	false,	true,	KEY_A);
	scantree_insert(&us_qwerty_set1, "\x9F",	false,	true,	KEY_S);
	scantree_insert(&us_qwerty_set1, "\xA0",	false,	true,	KEY_D);
	scantree_insert(&us_qwerty_set1, "\xA1",	false,	true,	KEY_F);
	scantree_insert(&us_qwerty_set1, "\xA2",	false,	true,	KEY_G);
	scantree_insert(&us_qwerty_set1, "\xA3",	false,	true,	KEY_H);
	scantree_insert(&us_qwerty_set1, "\xA4",	false,	true,	KEY_J);
	scantree_insert(&us_qwerty_set1, "\xA5",	false,	true,	KEY_K);
	scantree_insert(&us_qwerty_set1, "\xA6",	false,	true,	KEY_L);
	scantree_insert(&us_qwerty_set1, "\xA7",	false,	true,	KEY_SEMICOLON);
	scantree_insert(&us_qwerty_set1, "\xA8",	false,	true,	KEY_SINGLEQUOTE);
	scantree_insert(&us_qwerty_set1, "\xA9",	false,	true,	KEY_BACKTICK);
	scantree_insert(&us_qwerty_set1, "\xAA",	false,	true,	KEY_LSHIFT);
	scantree_insert(&us_qwerty_set1, "\xAB",	false,	true,	KEY_BACKSLASH);
	scantree_insert(&us_qwerty_set1, "\xAC",	false,	true,	KEY_Z);
	scantree_insert(&us_qwerty_set1, "\xAD",	false,	true,	KEY_X);
	scantree_insert(&us_qwerty_set1, "\xAE",	false,	true,	KEY_C);
	scantree_insert(&us_qwerty_set1, "\xAF",	false,	true,	KEY_V);
	scantree_insert(&us_qwerty_set1, "\xB0",	false,	true,	KEY_B);
	scantree_insert(&us_qwerty_set1, "\xB1",	false,	true,	KEY_N);
	scantree_insert(&us_qwerty_set1, "\xB2",	false,	true,	KEY_M);
	scantree_insert(&us_qwerty_set1, "\xB3",	false,	true,	KEY_COMMA);
	scantree_insert(&us_qwerty_set1, "\xB4",	false,	true,	KEY_DOT);
	scantree_insert(&us_qwerty_set1, "\xB5",	false,	true,	KEY_SLASH);
	scantree_insert(&us_qwerty_set1, "\xB6",	false,	true,	KEY_RSHIFT);
	scantree_insert(&us_qwerty_set1, "\xB7",	false,	true,	KEY_NUM_STAR);
	scantree_insert(&us_qwerty_set1, "\xB8",	false,	true,	KEY_LEFTALT);
	scantree_insert(&us_qwerty_set1, "\xB9",	false,	true,	KEY_SPACE);
	scantree_insert(&us_qwerty_set1, "\xBA",	false,	true,	KEY_CAPSLOCK);
	scantree_insert(&us_qwerty_set1, "\xBB",	false,	true,	KEY_F1);
	scantree_insert(&us_qwerty_set1, "\xBC",	false,	true,	KEY_F2);
	scantree_insert(&us_qwerty_set1, "\xBD",	false,	true,	KEY_F3);
	scantree_insert(&us_qwerty_set1, "\xBE",	false,	true,	KEY_F4);
	scantree_insert(&us_qwerty_set1, "\xBF",	false,	true,	KEY_F5);
	scantree_insert(&us_qwerty_set1, "\xC0",	false,	true,	KEY_F6);
	scantree_insert(&us_qwerty_set1, "\xC1",	false,	true,	KEY_F7);
	scantree_insert(&us_qwerty_set1, "\xC2",	false,	true,	KEY_F8);
	scantree_insert(&us_qwerty_set1, "\xC3",	false,	true,	KEY_F9);
	scantree_insert(&us_qwerty_set1, "\xC4",	false,	true,	KEY_F10);
	scantree_insert(&us_qwerty_set1, "\xC5",	false,	true,	KEY_NUMBERLOCK);
	scantree_insert(&us_qwerty_set1, "\xC6",	false,	true,	KEY_SCROLLLOCK);
	scantree_insert(&us_qwerty_set1, "\xC7",	false,	true,	KEY_NUM_7);
	scantree_insert(&us_qwerty_set1, "\xC8",	false,	true,	KEY_NUM_8);
	scantree_insert(&us_qwerty_set1, "\xC9",	false,	true,	KEY_NUM_9);
	scantree_insert(&us_qwerty_set1, "\xCA",	false,	true,	KEY_NUM_MINUS);
	scantree_insert(&us_qwerty_set1, "\xCB",	false,	true,	KEY_NUM_4);
	scantree_insert(&us_qwerty_set1, "\xCC",	false,	true,	KEY_NUM_5);
	scantree_insert(&us_qwerty_set1, "\xCD",	false,	true,	KEY_NUM_6);
	scantree_insert(&us_qwerty_set1, "\xCE",	false,	true,	KEY_NUM_PLUS);
	scantree_insert(&us_qwerty_set1, "\xCF",	false,	true,	KEY_NUM_1);
	scantree_insert(&us_qwerty_set1, "\xD0",	false,	true,	KEY_NUM_2);
	scantree_insert(&us_qwerty_set1, "\xD1",	false,	true,	KEY_NUM_3);
	scantree_insert(&us_qwerty_set1, "\xD2",	false,	true,	KEY_NUM_0);
	scantree_insert(&us_qwerty_set1, "\xD3",	false,	true,	KEY_NUM_DOT);
	scantree_insert(&us_qwerty_set1, "\xD7",	false,	true,	KEY_F11);
	scantree_insert(&us_qwerty_set1, "\xD8",	false,	true,	KEY_F12);
	scantree_insert(&us_qwerty_set1, "\xE0\x1C",	true,	false,	KEY_NUM_ENTER);
	scantree_insert(&us_qwerty_set1, "\xE0\x1D",	true,	false,	KEY_RIGHTCONTROL);
	scantree_insert(&us_qwerty_set1, "\xE0\x35",	true,	false,	KEY_NUM_SLASH);
	scantree_insert(&us_qwerty_set1, "\xE0\x38",	true,	false,	KEY_RIGHTALT);
	scantree_insert(&us_qwerty_set1, "\xE0\x47",	true,	false,	KEY_HOME);
	scantree_insert(&us_qwerty_set1, "\xE0\x48",	true,	false,	KEY_CURSORUP);
	scantree_insert(&us_qwerty_set1, "\xE0\x49",	true,	false,	KEY_PAGEUP);
	scantree_insert(&us_qwerty_set1, "\xE0\x4B",	true,	false,	KEY_CURSORLEFT);
	scantree_insert(&us_qwerty_set1, "\xE0\x4D",	true,	false,	KEY_CURSORRIGHT);
	scantree_insert(&us_qwerty_set1, "\xE0\x4F",	true,	false,	KEY_END);
	scantree_insert(&us_qwerty_set1, "\xE0\x50",	true,	false,	KEY_CURSORDOWN);
	scantree_insert(&us_qwerty_set1, "\xE0\x51",	true,	false,	KEY_PAGEDOWN);
	scantree_insert(&us_qwerty_set1, "\xE0\x52",	true,	false,	KEY_INSERT);
	scantree_insert(&us_qwerty_set1, "\xE0\x53",	true,	false,	KEY_DELETE);
	scantree_insert(&us_qwerty_set1, "\xE0\x5B",	true,	false,	KEY_LEFTGUI);
	scantree_insert(&us_qwerty_set1, "\xE0\x5C",	true,	false,	KEY_RIGHTGUI);
	scantree_insert(&us_qwerty_set1, "\xE0\x5D",	true,	false,	KEY_APPS);
	scantree_insert(&us_qwerty_set1, "\xE0\x9C",	false,	true,	KEY_NUM_ENTER);
	scantree_insert(&us_qwerty_set1, "\xE0\x9D",	false,	true,	KEY_RIGHTCONTROL);
	scantree_insert(&us_qwerty_set1, "\xE0\xB5",	false,	true,	KEY_NUM_SLASH);
	scantree_insert(&us_qwerty_set1, "\xE0\xB8",	false,	true,	KEY_RIGHTALT);
	scantree_insert(&us_qwerty_set1, "\xE0\xC7",	false,	true,	KEY_HOME);
	scantree_insert(&us_qwerty_set1, "\xE0\xC8",	false,	true,	KEY_CURSORUP);
	scantree_insert(&us_qwerty_set1, "\xE0\xC9",	false,	true,	KEY_PAGEUP);
	scantree_insert(&us_qwerty_set1, "\xE0\xCB",	false,	true,	KEY_CURSORLEFT);
	scantree_insert(&us_qwerty_set1, "\xE0\xCD",	false,	true,	KEY_CURSORRIGHT);
	scantree_insert(&us_qwerty_set1, "\xE0\xCF",	false,	true,	KEY_END);
	scantree_insert(&us_qwerty_set1, "\xE0\xD0",	false,	true,	KEY_CURSORDOWN);
	scantree_insert(&us_qwerty_set1, "\xE0\xD1",	false,	true,	KEY_PAGEDOWN);
	scantree_insert(&us_qwerty_set1, "\xE0\xD2",	false,	true,	KEY_INSERT);
	scantree_insert(&us_qwerty_set1, "\xE0\xD3",	false,	true,	KEY_DELETE);
	scantree_insert(&us_qwerty_set1, "\xE0\xDB",	false,	true,	KEY_LEFTGUI);
	scantree_insert(&us_qwerty_set1, "\xE0\xDC",	false,	true,	KEY_RIGHTGUI);
	scantree_insert(&us_qwerty_set1, "\xE0\xDD",	false,	true,	KEY_APPS);
	scantree_insert(&us_qwerty_set1, "\xE0\x2A\xE0\x37",	true,	false,	KEY_PRINTSCREEN);
	scantree_insert(&us_qwerty_set1, "\xE0\xB7\xE0\xAA",	false,	true,	KEY_PRINTSCREEN);
	scantree_insert(&us_qwerty_set1, "\xE1\x1D\x45\xE1\x9D\xC5",	true,	true,	KEY_PAUSE);

	memset(us_qwerty_set1_ascii, 0, sizeof(us_qwerty_set1_ascii));

	us_qwerty_set1_ascii[KEY_1] 		= (struct key_to_ascii){'1', '!'};
	us_qwerty_set1_ascii[KEY_2] 		= (struct key_to_ascii){'2', '@'};
	us_qwerty_set1_ascii[KEY_3] 		= (struct key_to_ascii){'3', '#'};
	us_qwerty_set1_ascii[KEY_4] 		= (struct key_to_ascii){'4', '$'};
	us_qwerty_set1_ascii[KEY_5] 		= (struct key_to_ascii){'5', '%'};
	us_qwerty_set1_ascii[KEY_6] 		= (struct key_to_ascii){'6', '^'};
	us_qwerty_set1_ascii[KEY_7] 		= (struct key_to_ascii){'7', '&'};
	us_qwerty_set1_ascii[KEY_8] 		= (struct key_to_ascii){'8', '*'};
	us_qwerty_set1_ascii[KEY_9] 		= (struct key_to_ascii){'9', '('};
	us_qwerty_set1_ascii[KEY_0] 		= (struct key_to_ascii){'0', ')'};
	us_qwerty_set1_ascii[KEY_MINUS] 	= (struct key_to_ascii){'-', '_'};
	us_qwerty_set1_ascii[KEY_EQUAL] 	= (struct key_to_ascii){'=', '+'};
	us_qwerty_set1_ascii[KEY_BACKSPACE] = (struct key_to_ascii){'\b', '\b'};
	us_qwerty_set1_ascii[KEY_TAB] 		= (struct key_to_ascii){'\t', '\t'};
	us_qwerty_set1_ascii[KEY_Q] 		= (struct key_to_ascii){'q', 'Q'};
	us_qwerty_set1_ascii[KEY_W] 		= (struct key_to_ascii){'w', 'W'};
	us_qwerty_set1_ascii[KEY_E] 		= (struct key_to_ascii){'e', 'E'};
	us_qwerty_set1_ascii[KEY_R] 		= (struct key_to_ascii){'r', 'R'};
	us_qwerty_set1_ascii[KEY_T] 		= (struct key_to_ascii){'t', 'T'};
	us_qwerty_set1_ascii[KEY_Y] 		= (struct key_to_ascii){'y', 'Y'};
	us_qwerty_set1_ascii[KEY_U] 		= (struct key_to_ascii){'u', 'U'};
	us_qwerty_set1_ascii[KEY_I] 		= (struct key_to_ascii){'i', 'I'};
	us_qwerty_set1_ascii[KEY_O] 		= (struct key_to_ascii){'o', 'O'};
	us_qwerty_set1_ascii[KEY_P] 		= (struct key_to_ascii){'p', 'P'};
	us_qwerty_set1_ascii[KEY_LBRACKET] 	= (struct key_to_ascii){'[', '{'};
	us_qwerty_set1_ascii[KEY_RBRACKET] 	= (struct key_to_ascii){']', '}'};
	us_qwerty_set1_ascii[KEY_ENTER] 	= (struct key_to_ascii){'\n', '\n'};
	us_qwerty_set1_ascii[KEY_A] 		= (struct key_to_ascii){'a', 'A'};
	us_qwerty_set1_ascii[KEY_S] 		= (struct key_to_ascii){'s', 'S'};
	us_qwerty_set1_ascii[KEY_D] 		= (struct key_to_ascii){'d', 'D'};
	us_qwerty_set1_ascii[KEY_F] 		= (struct key_to_ascii){'f', 'F'};
	us_qwerty_set1_ascii[KEY_G] 		= (struct key_to_ascii){'g', 'G'};
	us_qwerty_set1_ascii[KEY_H] 		= (struct key_to_ascii){'h', 'H'};
	us_qwerty_set1_ascii[KEY_J] 		= (struct key_to_ascii){'j', 'J'};
	us_qwerty_set1_ascii[KEY_K] 		= (struct key_to_ascii){'k', 'K'};
	us_qwerty_set1_ascii[KEY_L] 		= (struct key_to_ascii){'l', 'L'};
	us_qwerty_set1_ascii[KEY_SEMICOLON] = (struct key_to_ascii){';', ':'};
	us_qwerty_set1_ascii[KEY_SINGLEQUOTE] = (struct key_to_ascii){'\'', '"'};
	us_qwerty_set1_ascii[KEY_BACKTICK] 	= (struct key_to_ascii){'`', '~'};
	us_qwerty_set1_ascii[KEY_BACKSLASH] = (struct key_to_ascii){'\\', '|'};
	us_qwerty_set1_ascii[KEY_Z] 		= (struct key_to_ascii){'z', 'Z'};
	us_qwerty_set1_ascii[KEY_X] 		= (struct key_to_ascii){'x', 'X'};
	us_qwerty_set1_ascii[KEY_C] 		= (struct key_to_ascii){'c', 'C'};
	us_qwerty_set1_ascii[KEY_V] 		= (struct key_to_ascii){'v', 'V'};
	us_qwerty_set1_ascii[KEY_B] 		= (struct key_to_ascii){'b', 'B'};
	us_qwerty_set1_ascii[KEY_N] 		= (struct key_to_ascii){'n', 'N'};
	us_qwerty_set1_ascii[KEY_M] 		= (struct key_to_ascii){'m', 'M'};
	us_qwerty_set1_ascii[KEY_COMMA] 	= (struct key_to_ascii){',', '<'};
	us_qwerty_set1_ascii[KEY_DOT] 		= (struct key_to_ascii){'.', '>'};
	us_qwerty_set1_ascii[KEY_SLASH] 	= (struct key_to_ascii){'/', '?'};
	us_qwerty_set1_ascii[KEY_NUM_STAR] 	= (struct key_to_ascii){'*', '*'};
	us_qwerty_set1_ascii[KEY_SPACE] 	= (struct key_to_ascii){' ', ' '};
	us_qwerty_set1_ascii[KEY_NUM_7] 	= (struct key_to_ascii){'7', 0};
	us_qwerty_set1_ascii[KEY_NUM_8] 	= (struct key_to_ascii){'8', 0};
	us_qwerty_set1_ascii[KEY_NUM_9] 	= (struct key_to_ascii){'9', 0};
	us_qwerty_set1_ascii[KEY_NUM_MINUS] = (struct key_to_ascii){'-', '-'};
	us_qwerty_set1_ascii[KEY_NUM_4] 	= (struct key_to_ascii){'4', 0};
	us_qwerty_set1_ascii[KEY_NUM_5] 	= (struct key_to_ascii){'5', 0};
	us_qwerty_set1_ascii[KEY_NUM_6] 	= (struct key_to_ascii){'6', 0};
	us_qwerty_set1_ascii[KEY_NUM_PLUS] 	= (struct key_to_ascii){'+', '+'};
	us_qwerty_set1_ascii[KEY_NUM_1] 	= (struct key_to_ascii){'1', 0};
	us_qwerty_set1_ascii[KEY_NUM_2] 	= (struct key_to_ascii){'2', 0};
	us_qwerty_set1_ascii[KEY_NUM_3] 	= (struct key_to_ascii){'3', 0};
	us_qwerty_set1_ascii[KEY_NUM_0] 	= (struct key_to_ascii){'0', 0};
	us_qwerty_set1_ascii[KEY_NUM_DOT] 	= (struct key_to_ascii){'.', 0};
	us_qwerty_set1_ascii[KEY_NUM_ENTER] = (struct key_to_ascii){'\n', '\n'};
	us_qwerty_set1_ascii[KEY_NUM_SLASH] = (struct key_to_ascii){'/', '/'};
}
