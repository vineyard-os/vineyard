; void msr_write(uint32_t msr, uint64_t value)
[global efi_msr_write]
efi_msr_write:
	push rbp
	mov rbp, rsp

	; move the higher 32 bits of value to edx
	mov eax, edx
	shr rdx, 32

	; write edx:eax into the MSR ecx
	wrmsr

	pop rbp
	ret

[global efi_msr_read]
efi_msr_read:
	push rbp
	mov rbp, rsp

	xor rax, rax
	xor rdx, rdx

	rdmsr

	shl rdx, 32
	or rax, rdx

	pop rbp
	ret
