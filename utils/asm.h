#pragma once

#include "common.h"

// EFLAGS flags

#define EFLAGS_CF (1 << 0)
// reserved (1)
#define EFLAGS_PF (1 << 2)
// reserved (0)
#define EFLAGS_AF (1 << 4)
// reserved (0)
#define EFLAGS_ZF (1 << 6)
#define EFLAGS_SF (1 << 7)
#define EFLAGS_TF (1 << 8)
#define EFLAGS_IF (1 << 9)
#define EFLAGS_DF (1 << 10)
#define EFLAGS_OF (1 << 11)
#define EFLAGS_IO_PRIV(lvl) ((lvl) << 12)
#define EFLAGS_NT (1 << 14)
// reserved (0)
#define EFLAGS_RF (1 << 16)
#define EFLAGS_VM (1 << 17)
#define EFLAGS_AC (1 << 18)
#define EFLAGS_VIF (1 << 19)
#define EFLAGS_VIP (1 << 20)
#define EFLAGS_ID (1 << 21)

// Modifies interrupt flag in EFLAGS
// Returns previous state
bool enable_interrupts();
bool disable_interrupts();

void set_int_flag(bool val);
bool get_int_flag();

// Atomic equivalent of:
// if (*ptr == cmp_val) *ptr = new_val;
// return *ptr;
ull cmp_xchg_8b(volatile ull* ptr, ull cmp_val, ull new_val);

inline ull atomic_read_8b(volatile ull* ptr) {
	return cmp_xchg_8b(ptr, 0, 0);
}

// Returns previous value
inline ull atomic_add_8b(volatile ull* ptr, ull addend) {
	ull val;
	while (1) {
		val = atomic_read_8b(ptr);
		ull new_val = val + addend;
		if (cmp_xchg_8b(ptr, val, new_val) == new_val)
			break;
	}
	return val;
}

ushort get_cs();
ushort get_ds();
ushort get_es();
ushort get_fs();
ushort get_gs();
ushort get_ss();
