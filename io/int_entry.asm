[bits 32]

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
	push ebp
	mov ebp, esp

	push eax
	push ecx
	push edx
	
	; set segments (we could get called from usermode)
	push ds
	push es
	mov eax, 0x10
	mov ds, ax
	mov es, ax
	
	; TODO: align stack to 16B before the call

	; Arguments
	push edi
	push esi
	push edx
	push ecx
	push ebx
	push dword[ebp - 4] ; pushed eax

	mov eax, [ebp + 4] ; Interrupt number pushed by int_entry_X
	call dword [int_handlers + eax*4] ; call C handler

	add esp, 6*4
	
	; return
	pop es
	pop ds
	pop edx
	pop ecx
	pop eax

	mov esp, ebp
	pop ebp
	add esp, 4 ; pop interrupt number
	iret

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
