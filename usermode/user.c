#include "common.h"
#include "user_syscalls.h"

int user_thread(void* arg) {
	user_sys_get_keyboard_focus();
	while (1) {
		struct keyboard_event event;
		if (!user_sys_get_key(&event)) {
			user_yield();
		} else {
			if (event.ascii) {
				char buf[2];
				buf[0] = event.ascii;
				buf[1] = 0;
				user_sys_print(buf);
			}
		}
	}
	return 0;
}

int user_main(void* arg) {
	user_sys_create_thread(user_thread, arg, "Keyboard thread");
	user_sys_print("Hello from user!\n");
	user_sys_test((int)arg, 2, 3, 4, 5);
	for (int i = 0; i < 10; i++) {
		user_sys_test(i, 2, 3, 4, 5);
		user_sleep(0.7);
	}
	return 4321;
}
