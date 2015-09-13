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

global cmp_xchg_8b
cmp_xchg_8b:
	push ebp
	mov ebp, esp
	push ebx
	push ecx
	push esi

	mov eax, [ebp + 0ch]
	mov edx, [ebp + 10h]
	mov ebx, [ebp + 14h]
	mov ecx, [ebp + 18h]

	mov esi, [ebp + 8]
	cmpxchg8b [esi]

	jnz not_changed
		; ZF = 1
		mov eax, ebx
		mov edx, ecx
	not_changed:

	pop esi
	pop ecx
	pop ebx
	mov esp, ebp
	pop ebp
	retn

%macro get 1
	xor eax, eax
	mov ax, %1
	retn
%endmacro

global get_cs
get_cs:
	get cs

global get_ds
get_ds:
	get ds

global get_es
get_es:
	get es

global get_fs
get_fs:
	get fs

global get_gs
get_gs:
	get gs

global get_ss
get_ss:
	get ss

