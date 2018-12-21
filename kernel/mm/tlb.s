[global mm_tlb_invlpg]
mm_tlb_invlpg:
	push rbp
	mov rbp, rsp
	invlpg [rdi]
	pop rbp
	ret
