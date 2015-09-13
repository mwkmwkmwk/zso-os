%ifndef CONTEXT_INC
%define CONTEXT_INC

; Update context.h after changing anything here!
struc context
	.eax: resd 1
	.ebx: resd 1
	.ecx: resd 1
	.edx: resd 1
	.esi: resd 1
	.edi: resd 1
	.ebp: resd 1
	.ds:  resd 1
	.es:  resd 1
	.fs:  resd 1
	.gs:  resd 1

	; layed out according to priv. changing iret
	.eip: resd 1 ; must be together with cs
	.cs:  resd 1

	.eflags: resd 1

	.esp: resd 1 ; must be together with ss, for use of lss
	.ss:  resd 1

	alignb 16
	.fxstate: resb 512 ; state from fxsave

endstruc

%ifndef CONTEXT_SOURCE
	extern save_context
	extern load_context
%endif ; CONTEXT_SOURCE

%endif ; CONTEXT_INC
