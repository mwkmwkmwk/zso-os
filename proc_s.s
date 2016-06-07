.global switch_process
switch_process:
movl 4(%esp), %eax
pushl %edi
pushl %esi
pushl %ebp
movl tss+4, %edx
pushl %edx
movl %cr3, %edx
pushl %edx
movl proc_cur, %ecx
mov %esp, (%ecx)
mov (%eax), %esp
mov %eax, proc_cur
popl %edx
movl %edx, %cr3
popl %edx
movl %edx, tss+4
popl %ebp
popl %esi
popl %edi
ret
