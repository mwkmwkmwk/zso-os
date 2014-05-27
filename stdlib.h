#ifndef STDLIB_H
#define STDLIB_H

#include <stdint.h>

// abs of int32_t to uint32_t that is correct even for values
// that would overflow in normal abs (i.e. INT_MIN)
extern uint32_t abs_i32_to_u32(int32_t);

#endif
