[global gdt_gdtr]
gdt_gdtr:
	push rbp
	mov rbp, rsp
	lgdt [rdi]
	mov ax, 0x10
	mov ss, ax
	mov ax, 0
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov rax, qword .apply
	push 0x08
	push rax
	o64 retf
.apply
	pop rbp
	ret
