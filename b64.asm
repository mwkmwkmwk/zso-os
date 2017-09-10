org 0x90600
bits 64

start:

mov rax, 0xffff8000000b8000
;times 0x50 nop
;mov rax, 0x00b8000
mov rdx, 0x0c340c360c4d0c50
mov [rax+160], rdx

jmp [0xffff800000100018]

align 0x200
