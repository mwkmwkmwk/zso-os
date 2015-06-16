.text

asm_sys_hello:
	pushl %eax
	pushl %ecx
	pushl %edx
	pushl %ds
	pushl %es
	movl $0x10, %eax
	movl %eax, %ds
	movl %eax, %es
	call sys_hello
	popl %es
	popl %ds
	popl %edx
	popl %ecx
	popl %eax
	iret

.global asm_sys_hello

asm_div_zero:
	pushl %eax
	pushl %ecx
	pushl %edx
	call div_zero
	popl %edx
	popl %ecx
	popl %eax
	iret

.global asm_div_zero

asm_err_page:
	pushl %eax
	pushl %ecx
	pushl %edx
	call err_page
	popl %edx
	popl %ecx
	popl %eax
	iret

.global asm_err_page

asm_err_gp:
	pushl %eax
	pushl %ecx
	pushl %edx
	call err_gp
	popl %edx
	popl %ecx
	popl %eax
	iret

.global asm_err_gp

asm_irq1:
	pushl %eax
	pushl %ecx
	pushl %edx
	call irq1
	popl %edx
	popl %ecx
	popl %eax
	iret

.global asm_irq1
