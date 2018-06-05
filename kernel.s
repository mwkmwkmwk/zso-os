.text

.global _start

_start:

movabsq $kbss_start, %rdi
movabsq $kbss_end, %rcx
subq %rdi, %rcx
xorb %al, %al
repz stosb

movabsq $stack, %rsp

call main

cli
hlt

.section .bss

.skip 0x1000
.global stack
stack:
