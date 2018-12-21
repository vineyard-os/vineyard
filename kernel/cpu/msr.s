; void msr_write(uint32_t msr, uint64_t value)
[global msr_write]
msr_write:
	push rbp
	mov rbp, rsp

	; move msr number to ecx
	mov rcx, rdi

	; move the lower 32 bits of value to eax
	mov rdx, rsi

	; move the higher 32 bits of value to edx
	shr rdx, 32

	mov rax, rsi

	; write edx:eax into the MSR ecx
	wrmsr
	pop rbp
	ret

[global msr_read]
msr_read:
	push rbp

	mov rbp, rsp
	mov rcx, rdi
	xor rax, rax
	xor rdx, rdx

	rdmsr

	shl rdx, 32
	or rax, rdx

	pop rbp
	ret
