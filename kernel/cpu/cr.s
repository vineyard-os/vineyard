[global cr0_read]
cr0_read:
	push rbp
	mov rbp, rsp
	mov rax, cr0
	pop rbp
	ret

[global cr0_write]
cr0_write:
	push rbp
	mov rbp, rsp
	mov cr0, rdi
	pop rbp
	ret

[global cr2_read]
cr2_read:
	push rbp
	mov rbp, rsp
	mov rax, cr2
	pop rbp
	ret

[global cr3_read]
cr3_read:
	push rbp
	mov rbp, rsp
	mov rax, cr3
	pop rbp
	ret

[global cr3_write]
cr3_write:
	push rbp
	mov rbp, rsp
	mov cr3, rdi
	pop rbp
	ret

[global cr4_read]
cr4_read:
	push rbp
	mov rbp, rsp
	mov rax, cr4
	pop rbp
	ret

[global cr4_write]
cr4_write:
	push rbp
	mov rbp, rsp
	mov cr4, rdi
	pop rbp
	ret
