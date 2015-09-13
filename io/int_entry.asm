[bits 32]

%include "threading/context.inc"

section .text

extern int_handlers

; Interrupt entries
%macro int_entry 1
	int_entry_%1:
	push %1
	jmp common_int_entry
%endmacro

%assign i 0
%rep 256
	int_entry %[i]
	%assign i i+1
%endrep
%undef i

common_int_entry:

	.saved_context_ptr equ -context_size-4 ; without segment, assuming flat addressing
	.saved_context     equ -context_size
	.saved_ebp         equ +0h
	.int_num           equ +4h ; Interrupt number pushed by int_entry_X
	;.err_code          equ +8h ; TODO: Handle this for certain exceptions
	.saved_eip         equ +8h
	.saved_cs          equ +0ch
	.saved_eflags      equ +10h
	.saved_esp         equ +14h ; Valid only on priv changing interrupt
	.saved_ss          equ +18h

	cli
	push ebp
	mov ebp, esp

	sub esp, context_size + 4
	mov [ebp + .saved_context_ptr], ebp
	add dword [ebp + .saved_context_ptr], .saved_context

	; save context
	;(struct context* context, ushort ss, uint esp, ushort cs, uint eip, ushort ds, ushort es, uint ebp, uint eflags
	push dword [ebp + .saved_eflags]
	push dword [ebp + .saved_ebp]
	; set segments (we could get called from usermode)
	push es
	push ds
	
	push dword 0x10
	pop ds
	push dword 0x10
	pop es
	
	push dword [ebp + .saved_eip]
	push dword [ebp + .saved_cs]

	; Check if priviledges have changed
	push eax
	mov ax, cs
	xor eax, [ebp + .saved_cs]
	and eax, 0b11
	cmp ax, 0 ; Assuming that we only use ring0 and ring3
	pop eax
	jz .no_priv_change
		push dword [ebp + .saved_esp]
		push dword [ebp + .saved_ss]
		jmp .no_priv_change_cont
	.no_priv_change:
		push ebp
		add dword [esp], .saved_eflags + 4 ; esp before the interrupt
		push ss
	.no_priv_change_cont:
	
	push ebp
	add dword [esp], .saved_context
	call save_context
	add esp, 9*4

	; arguments
	lea eax, [ebp + .saved_context_ptr]
	push eax

	; call C handler
	mov eax, [ebp + .int_num]
	call dword [int_handlers + eax*4] ; TODO: align stack here
	add esp, 4

	; return
	push dword [ebp + .saved_context_ptr]
	call load_context
	; should never happen
	cli
	hlt

section .rodata

; Interrupt entries pointers
global int_entries_ptrs
int_entries_ptrs:
%assign i 0
%rep 256
	dd int_entry_%[i]
	%assign i i+1
%endrep
%undef i
