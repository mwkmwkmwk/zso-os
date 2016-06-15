.section mb_header, "a"
.long 0x1BADB002
.long 3
.long -3-0x1BADB002

.section .text

.global _start
_start:
movl $stack_end, %esp
call main
cli
hlt


.global irq0_asm
irq0_asm:
pushl %ds
pushl %es
pushl %edx
pushl %ecx
pushl %eax
movl $0x10, %eax
movl %eax, %ds
movl %eax, %es
call irq0
popl %eax
popl %ecx
popl %edx
popl %es
popl %ds
iretl

.global irq1_asm
irq1_asm:
pushl %ds
pushl %es
pushl %edx
pushl %ecx
pushl %eax
movl $0x10, %eax
movl %eax, %ds
movl %eax, %es
call irq1
popl %eax
popl %ecx
popl %edx
popl %es
popl %ds
iretl

.global pagefault_asm
pagefault_asm:
pushl %ds
pushl %es
pushl %edi
pushl %esi
pushl %ebp
pushl %ebx
pushl %edx
pushl %ecx
pushl %eax
movl $0x10, %eax
movl %eax, %ds
movl %eax, %es
movl %esp, %ecx
movl %cr2, %eax
pushl %eax
pushl %ecx
call pagefault
popl %eax
popl %eax
popl %eax
popl %ecx
popl %edx
popl %ebx
popl %ebp
popl %esi
popl %edi
popl %es
popl %ds
iretl

.global syscall_asm
syscall_asm:
pushl %ds
pushl %es
pushl %edi
pushl %esi
pushl %ebp
pushl %ebx
pushl %edx
pushl %ecx
pushl %eax
movl $0x10, %eax
movl %eax, %ds
movl %eax, %es
pushl %esp
call syscall
popl %eax
popl %eax
popl %ecx
popl %edx
popl %ebx
popl %ebp
popl %esi
popl %edi
popl %es
popl %ds
iretl

.global boot_ap
boot_ap:
.code16
movw $0x900, %ax
movw %ax, %ds
movl $gdt, 4
movw $0x2f, 2
lgdtl 2
movl %cr0, %eax
orl $1, %eax
movl %eax, %cr0
movw $0x10, %ax
movw %ax, %ds
movw %ax, %es
movw %ax, %ss
ljmpl $0x08, $boot_ap_pm
.global boot_ap_end
boot_ap_end:
.code32

boot_ap_pm:
movl $stack2_end, %esp
call printf
pushl $hello_ap_end-hello_ap
pushl $hello_ap
pushl $0xb80a0
call memcpy
cli
hlt

.section .rodata

hello_ap:
.byte 'H', 0x0f
.byte 'e', 0x0f
.byte 'l', 0x0f
.byte 'l', 0x0f
.byte 'o', 0x0f
.byte ',', 0x0f
.byte ' ', 0x0f
.byte 'A', 0x0f
.byte 'P', 0x0f
.byte '!', 0x0f
hello_ap_end:

.section .bss
.skip 0x1000
.global stack_end
stack_end:

.skip 0x1000
.global stack2_end
stack2_end:

.section .data
.align 0x1000
.global user
user:
.incbin "user"
.global user2
user2:
.incbin "user2"
