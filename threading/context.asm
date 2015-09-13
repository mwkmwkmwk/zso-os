[bits 32]

%define CONTEXT_SOURCE
%include "threading/context.inc"

; (struct context* context, uint ss, uint esp, uint cs, uint eip, uint ds, uint es, uint ebp, uint eflags)
global save_context
save_context:
	push ebp
	mov ebp, esp

	push eax
	mov eax, [ebp + 8] ; &context
	pop dword [eax + context.eax]
	mov dword [eax + context.ebx], ebx
	mov dword [eax + context.ecx], ecx
	mov dword [eax + context.edx], edx
	mov dword [eax + context.esi], esi
	mov dword [eax + context.edi], edi
	add esp, 0ch
	pop dword [eax + context.ss]
	pop dword [eax + context.esp]
	pop dword [eax + context.cs]
	pop dword [eax + context.eip]
	pop dword [eax + context.ds]
	pop dword [eax + context.es]
	pop dword [eax + context.ebp]
	pop dword [eax + context.eflags]

	fxsave [eax + context.fxstate]
	xor edx, edx
	mov dx, fs
	mov [eax + context.fs], edx
	mov dx, gs
	mov [eax + context.gs], edx

	mov esp, ebp
	pop ebp
	retn

global load_context
load_context:
	mov ebp, [esp + 4] ; ebp = &context
	fxrstor [ebp + context.fxstate]

	; Check if privileges have changed
	mov ax, cs
	xor eax, [ebp + context.cs]
	and eax, 0b11
	cmp ax, 0
; !!!! No arithmetic after this point (until .no_priv_change) !!!!
	jnz .priv_change
		mov ax, ss
		mov esi, esp
		lss esp, [ebp + context.esp]
		
		; Prepare old stack top for iret
		push dword [ebp + context.eflags]
		push dword [ebp + context.cs]
		push dword [ebp + context.eip]

		mov ss, ax ; blocks interrupts until the next instruction
		mov esp, esi
	.priv_change:
	mov esp, ebp
	pop eax
	pop ebx
	pop ecx
	pop edx
	pop esi
	pop edi
	pop ebp
	pop ds
	pop es
	pop fs
	pop gs

	jz .no_priv_change
		iret
	.no_priv_change:
		add esp, 0ch
		lss esp, [esp]
		sub esp, 0ch
		iret
