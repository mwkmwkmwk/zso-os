[bits 32]

section .text

extern sys_hello
extern div_zero
extern err_page
extern err_gp
extern irq1

global asm_sys_hello
asm_sys_hello:
	push eax
	push ecx
	push edx
	push ds
	push es
	mov eax, 0x10
	mov ds, ax
	mov es, ax
	call sys_hello
	pop es
	pop ds
	pop edx
	pop ecx
	pop eax
	iret

global asm_div_zero
asm_div_zero:
	push eax
	push ecx
	push edx
	call div_zero
	pop edx
	pop ecx
	pop eax
	iret

global asm_err_page
asm_err_page:
	push eax
	push ecx
	push edx
	call err_page
	pop edx
	pop ecx
	pop eax
	iret


global asm_err_gp
asm_err_gp:
	push eax
	push ecx
	push edx
	call err_gp
	pop edx
	pop ecx
	pop eax
	iret


global asm_irq1
asm_irq1:
	push eax
	push ecx
	push edx
	call irq1
	pop edx
	pop ecx
	pop eax
	iret
