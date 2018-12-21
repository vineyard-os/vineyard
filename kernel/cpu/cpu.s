[global cpu_get]
cpu_get:
	push rbp
	mov rbp, rsp
	mov rax, [gs:0]
	pop rbp
	ret
