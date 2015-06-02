.section .text

.global framebuffer
framebuffer = 0xb8000

.global _start
_start:
movl $end_stack, %esp
call main
cli
hlt

.section .rodata

.balign 4
multiboot_header:
.long 0x1BADB002
.long 3
.long -0x1badb002-3

.section .bss

stack:
.skip 0x10000
end_stack:
