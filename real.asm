resb 0x1f1
db 3
dw 0
dd 0
dw 0
dw 0
dw 0
dw 0xaa55
jmp short startme
db "HdrS"
dw 0x200
dd 0
dw 0x1000
dw 0
db 0
db 1
dw 0
dd 0
dd 0
dd 0
dd 0

resb 0x50

startme:
mov ax, 0x9000
mov ds, ax
mov ax, 0xb800
mov es, ax

mov si, hello
mov di, 0
mov cx, hello_end - hello
mov ah, 0x0a
my_loop:
lodsb
stosw
loop my_loop

cli

mov eax, cr0
or eax, 1
mov cr0, eax

lgdt [ds:gdtr]
mov ax, 0x10
mov ds, ax
mov es, ax
mov ss, ax
mov ax, 0
mov fs, ax
mov gs, ax

o32 jmp dword 8:0x90400

hello:
db "Hello, world!"
hello_end:


align 4
resb 2
gdtr:
dw gdt_end - gdt - 1
dd 0x90000 + gdt


align 8
gdt:
dq 0
# gdt 1 - code
dw 0xffff
dw 0
db 0
db 0x9b
db 0xcf
db 0
# gdt 2 - data
dw 0xffff
dw 0
db 0
db 0x93
db 0xcf
db 0
# gdt 3 - code 64-bit
dw 0xffff
dw 0
db 0
db 0x9b
db 0xaf
db 0
gdt_end:

align 0x200
