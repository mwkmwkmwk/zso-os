org 0x123456789000
bits 64

mov rbx, hello
l:
mov eax, 0
mov dl, [rbx]
cmp dl, 0
je end
syscall
add rbx, 1
jmp l

end:
jmp end

hello:
db "Hello, user world!", 0xa, 0
