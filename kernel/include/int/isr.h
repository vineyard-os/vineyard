#pragma once

#include <stdint.h>
#include <vy.h>

typedef struct {
	uint64_t rax;
	uint64_t rbx;
	uint64_t rcx;
	uint64_t rdx;
	uint64_t rsi;
	uint64_t rdi;
	uint64_t rbp;
	uint64_t r8;
	uint64_t r9;
	uint64_t r10;
	uint64_t r11;
	uint64_t r12;
	uint64_t r13;
	uint64_t r14;
	uint64_t r15;

	uint64_t id;
	uint64_t error;

	uint64_t rip;
	uint64_t cs;
	uint64_t rflags;
	uint64_t rsp;
	uint64_t ss;
} vy_packed cpu_state_t;

void isr_dispatch(cpu_state_t *state);

void isr_handle_0(void);
void isr_handle_1(void);
void isr_handle_2(void);
void isr_handle_3(void);
void isr_handle_4(void);
void isr_handle_5(void);
void isr_handle_6(void);
void isr_handle_7(void);
void isr_handle_8(void);
void isr_handle_9(void);
void isr_handle_10(void);
void isr_handle_11(void);
void isr_handle_12(void);
void isr_handle_13(void);
void isr_handle_14(void);
void isr_handle_15(void);
void isr_handle_16(void);
void isr_handle_17(void);
void isr_handle_18(void);
void isr_handle_19(void);
void isr_handle_20(void);
void isr_handle_21(void);
void isr_handle_22(void);
void isr_handle_23(void);
void isr_handle_24(void);
void isr_handle_25(void);
void isr_handle_26(void);
void isr_handle_27(void);
void isr_handle_28(void);
void isr_handle_29(void);
void isr_handle_30(void);
void isr_handle_31(void);
