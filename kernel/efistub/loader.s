[global efi_loader_jump_to_kernel]
efi_loader_jump_to_kernel:
	mov rdi, rcx
	mov rsi, rdx
	mov rdx, r8
	mov rbp, (STACK + STACK_SIZE)
	mov rsp, rbp
	jmp r9

; SysV: rdi, rsi, rdx
; MS ABI: rcx, rdx, r8, r9
