#include "pit.h"

#include "common.h"
#include "io/interrupts.h"
#include "io/io.h"
#include "stdlib/math.h"
#include "utils/asm.h"

#define PIT_CMD(channel, access_mode, op_mode, use_bcd) \
	(((channel) << 6) + ((access_mode) << 4) + ((op_mode) << 1) + (use_bcd))
#define PIT_FREQ 1193182 // Hz

void init_pit(double ticks_per_sec) {
	ushort desired_freq = PIT_FREQ / ticks_per_sec;
	// Limit frequency to supported range
	ushort reload_val = max(min(desired_freq, 0x10000), 1);
	reload_val &= ~1;
	bool iflag = disable_interrupts();
	outb(PIT_CMD_PORT, PIT_CMD(0b00, 0b11, 0b010, 0));
	outb(PIT_CHANNEL_0_PORT, reload_val & 0xFF);
	outb(PIT_CHANNEL_0_PORT, (reload_val >> 8) & 0xFF);
	set_int_flag(iflag);
	enable_pic_interrupt(INT_PIT);
}
