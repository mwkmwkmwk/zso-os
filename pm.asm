org 0x90400
bits 32
;%define HUGEPAGES
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
%ifdef HUGEPAGES
mov eax, 0x00000083
mov edx, 0
.loop:
mov dword [0x8f000+ebx], eax
mov dword [0x8f004+ebx], edx
add eax, 0x40000000
adc edx, 0
add ebx, 8
loop .loop

%else
mov ebx, 0
mov dword [0x8f000+ebx], 0x8c003
mov dword [0x8f004+ebx], 0
add ebx, 24
mov dword [0x8f000+ebx], 0x8d003
mov dword [0x8f004+ebx], 0

mov ecx, 0x200
mov ebx, 0
mov eax, 0x00000083
mov edx, 0
.loop_lower:
mov dword [0x8c000+ebx], eax
mov dword [0x8c004+ebx], edx
add eax, 0x200000
adc edx, 0
add ebx, 8
loop .loop_lower

;this really should be a nested loop

mov ecx, 0x200
mov ebx, 0
mov eax, 0x00000083 | 0xc0000000
mov edx, 0
.loop_upper:
mov dword [0x8d000+ebx], eax
mov dword [0x8d004+ebx], edx
add eax, 0x200000
adc edx, 0
add ebx, 8
loop .loop_upper


%endif

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
