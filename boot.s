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

.section .bss
.skip 0x1000
stack_end:
