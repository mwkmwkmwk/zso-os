.section mb_hdr, "a"

.balign 4
.long 0xE85250D6
.long 0
.long 16
.long (-0xE85250D6-16)&0xffffffff

.long 0x1BADB002
.long 3
.long (-0x1BADB002-3)&0xffffffff

.text
.global _start

_start:

movl $0x200, %ecx
movl $boot_ptl2, %edi
movl $0x83, %eax
pt_init_loop:
movl %eax, (%edi)
addl $0x200000, %eax
addl $8, %edi
loop pt_init_loop

movl $boot_ptl4, %eax
movl %eax, %cr3

movl $0xc0000080, %ecx
movl $0x101, %eax
movl $0, %edx
wrmsr

movl %cr4, %eax
orl $0x00000020, %eax
movl %eax, %cr4

movl %cr0, %eax
orl $0x80000000, %eax
movl %eax, %cr0

lgdt gdtr

leal kernel_img, %edi
ljmpl $0x08, $start64


.code64

start64:
movq 0x18(%rdi), %rdi
jmpq *%rdi

.section .data

.balign 8
.short 0
gdtr:
.short 0x17
.long gdt

.balign 8
gdt:
# 0
.long 0
.long 0
# 1 - kernel code
.long 0
.long 0x00a09b00


.section ptables, "aw"
.balign 0x1000
boot_ptl4:
.long boot_ptl3+3
.long 0
.skip 0x7f8
.long boot_ptl3+3
.long 0
.skip 0x7f8

boot_ptl3:
.long boot_ptl2+3
.long 0
.skip 0xff8

boot_ptl2:
.skip 0x1000

.section kernel, "aw"
kernel_img:
.incbin "kernel"
