org 0x90400
bits 32

start:

mov eax, 0
mov edi, 0x8e000
mov ecx, 0x2000/4
rep stosd
mov dword [0x8e000], 0x8f003
mov dword [0x8e004], 0
mov dword [0x8e800], 0x8f003
mov dword [0x8e804], 0
mov ecx, 0x200
mov ebx, 0
mov eax, 0x00000083
mov edx, 0
.loop:
mov dword [0x8f000+ebx], eax
mov dword [0x8f004+ebx], edx
add eax, 0x40000000
adc edx, 0
add ebx, 8
loop .loop
mov eax, 0x8e000
mov cr3, eax
mov eax, cr4
or eax, 0x220
mov cr4, eax

mov ecx, 0xc0000080
rdmsr
or eax, 0x100
wrmsr
mov eax, cr0
or eax, 0x80000000
mov cr0, eax

jmp 0x18:0x90600


mov eax, 0xb8000
mov [ds:eax+160], dword 0x0c320c33
hlt

align 0x200
