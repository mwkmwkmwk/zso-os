[bits 32]

section .text

global enable_interrupts
enable_interrupts:
	call get_int_flag
	push eax
	push 1
	call set_int_flag
	add esp, 4
	pop eax
	retn

global disable_interrupts
disable_interrupts:
	call get_int_flag
	push eax
	push 0
	call set_int_flag
	add esp, 4
	pop eax
	retn

global set_int_flag
set_int_flag:
	cmp dword[esp + 8], 0
	jz zero_out
		sti
		retn
	zero_out:
		cli
		retn

global get_int_flag
get_int_flag:
	pushf
	pop eax
	and eax, 200h
	retn
