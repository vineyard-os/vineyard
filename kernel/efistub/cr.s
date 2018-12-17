[global efi_cr0_read]
efi_cr0_read:
	push rbp
	mov rbp, rsp
	mov rax, cr0
	pop rbp
	ret

[global efi_cr0_write]
efi_cr0_write:
	push rbp
	mov rbp, rsp
	mov cr0, rcx
	pop rbp
	ret

[global efi_cr3_read]
efi_cr3_read:
	push rbp
	mov rbp, rsp
	mov rax, cr3
	pop rbp
	ret
