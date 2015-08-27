[bits 32]

section .text

extern main

global framebuffer
framebuffer equ 0xb8000

global _start
_start:
	mov esp, end_stack
	push ebx
	call main
	cli
	hlt

section mb_hdr

align 4
multiboot_header:
	dd 0x1BADB002
	dd 3
	dd -0x1badb002-3

section .bss

stack:
resb 0x10000
global end_stack
end_stack:
