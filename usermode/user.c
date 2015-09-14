#include "common.h"
#include "user_syscalls.h"

int user_thread(void* arg) {
	user_sys_get_keyboard_focus();
	char line[50];
	user_sys_print("----------------------------\n");
	user_sys_print("Welcome in biedoshell v1.0\n");
	user_sys_print("----------------------------\n");
	while (1) {
		user_sys_print("> ");
		user_read_line(line, _countof(line));
		// user_sys_print("Received command: ");
		// user_sys_print(line);
		// user_sys_print("\n");
		if (user_strncmp(line, "echo ", 5) == 0) {
			user_sys_print(line + 5);
			user_sys_print("\n");
		} else if (user_strncmp(line, "reverse ", 8) == 0) {
			user_strrev(line + 8);
			user_sys_print(line + 8);
			user_sys_print("\n");
		} else if (line[0] == 0) {
			// nop
		} else {
			user_sys_print("Unknown command!\n");
		}
	}
	return 0;
}

int user_main(void* arg) {
	user_sys_print("Hello from user!\n");
	user_sys_test((int)arg, 2, 3, 4, 5);
	for (int i = 0; i < 10; i++) {
		user_sys_test(i, 2, 3, 4, 5);
		//user_sleep(0.2);
	}
	user_sys_print("Spawning shell...\n");
	user_sys_create_thread(user_thread, arg, "Shell thread");
	return 4321;
}
