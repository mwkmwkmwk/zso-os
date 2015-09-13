#pragma once

#include "common.h"
#include "threading/context.h"

typedef void timer_callback_t(struct context**);

void init_timers(void);
void sleep(ull ms);
ull get_current_time(void);
void add_tick_callback(timer_callback_t* f);
