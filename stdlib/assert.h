#pragma once

#include "panic.h"

#define __stringify(x) #x
#define stringify(x) __stringify(x)

#define assert(cond) do { if (!(cond)) panic("Assertion failed! " __FILE__ ":" stringify(__LINE__) ": " #cond); } while (false)