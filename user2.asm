org 0x23456789a000
bits 64

restart:

mov rbx, hello
l:
mov eax, 0
mov dl, [rbx]
cmp dl, 0
je sleep
syscall
add rbx, 1
jmp l

sleep:
mov rcx, 100000000
sleep_loop:
loop sleep_loop

jmp restart

hello:
db "Hello, user2 world!", 0xa, 0
