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
	shr eax, 9
	retn

global cmp_xchg_8b
cmp_xchg_8b:
	push ebp
	mov ebp, esp
	push ebx
	push ecx
	push esi

	mov eax, [ebp + 0ch] ; cmp_val
	mov edx, [ebp + 10h]
	mov ebx, [ebp + 14h] ; new_val
	mov ecx, [ebp + 18h]

	mov esi, [ebp + 8]
	cmpxchg8b [esi]

	jnz .not_changed
		; ZF = 1
		mov eax, ebx
		mov edx, ecx
	.not_changed:

	pop esi
	pop ecx
	pop ebx
	mov esp, ebp
	pop ebp
	retn

global cmp_xchg
cmp_xchg:
	push ebp
	mov ebp, esp
	push esi

	mov eax, [ebp + 0ch] ; cmp_val
	mov edx, [ebp + 10h] ; new_val

	mov esi, [ebp + 8]
	cmpxchg [esi], edx

	jnz .not_changed
		; ZF = 1
		mov eax, edx
	.not_changed:

	pop esi
	mov esp, ebp
	pop ebp
	retn

global atomic_set
atomic_set:
	mov eax, [esp + 8]
	mov edx, [esp + 4]
	xchg [edx], eax
	retn

global atomic_add
atomic_add:
	mov eax, [esp + 8]
	mov edx, [esp + 4]
	lock add [edx], eax
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

global fxstate_init
fxstate_init:
	mov eax, [esp + 4]
	fxsave [eax]
	retn
