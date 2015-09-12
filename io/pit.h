#pragma once

#define PIT_CHANNEL_0_PORT 0x40 // Channel 0 data port (read/write)
#define PIT_CHANNEL_1_PORT 0x41 // Channel 1 data port (read/write)
#define PIT_CHANNEL_2_PORT 0x42 // Channel 2 data port (read/write)
#define PIT_CMD_PORT 0x43       // Mode/Command register (write only, a read is ignored)

void init_pit(double ticks_per_sec);
