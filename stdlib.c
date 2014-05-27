#include "stdlib.h"

uint32_t abs_i32_to_u32(int32_t x) {
	if (x >= 0) {
		return (uint32_t)x;
	} else {
		return (uint32_t)((-1) * (x + 1)) + 1;
	}
}
