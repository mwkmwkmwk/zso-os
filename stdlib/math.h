#pragma once

// User/Kernel mode header

#define max(a, b) \
	({ __typeof__ (a) _a = (a); \
	   __typeof__ (b) _b = (b); \
	   _a > _b ? _a : _b; })

#define min(a, b) \
	({ __typeof__ (a) _a = (a); \
	   __typeof__ (b) _b = (b); \
	   _a < _b ? _a : _b; })

#define abs(x) \
	({ __typeof__ (x) _x = (x); \
	   _x < 0 ? -_x : _x; })

#define align_up(x, alignment) \
	((x) + ((alignment) - (x) % (alignment)) % (alignment))

#define align_down(x, alignment) \
	((x) - (x) % (alignment))
