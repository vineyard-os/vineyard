[global idt_load]
idt_load:
	push rbp
	mov rbp, rsp
	lidt [rdi]
	pop rbp
	ret
