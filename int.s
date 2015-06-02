.text

asm_sys_hello:
	pushl %eax
	pushl %ecx
	pushl %edx
	call sys_hello
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
