.section entry, "ax"
.global _start
_start:
movl $stack_end, %esp
call main
end:
jmp end

.section .bss
.skip 0x1000
stack_end:
