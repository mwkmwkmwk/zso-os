#include "common.h"
#include "user_syscalls.h"

int user_thread(void* arg) {
	user_sys_print("Hello from user thread!\n");
	return 0;
}

int user_main(void* arg) {
	user_create_thread(user_thread, arg, "A useless user thread");
	user_sys_print("Hello from user!\n");
	user_sys_test((int)arg, 2, 3, 4, 5);
	for (int i = 0; i < 10; i++) {
		user_sys_test(i, 2, 3, 4, 5);
		user_sleep(0.7);
	}
	return 4321;
}
