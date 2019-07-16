#include <boot/panic.h>
#include <misc/ubsan.h>
#include <stdio.h>
#include <vy.h>

no_warn(-Wunused-parameter)

static const char *type_check_kinds[] = {
	"load of",
	"store to",
	"reference binding to",
	"member access within",
	"member call on",
	"constructor call on",
	"downcast of",
	"downcast of",
	"upcast of",
	"cast to virtual base of",
	"_Nonnull binding to",
	"dynamic operation on",
};

static const char *ubsan_type_check_kind(uint8_t index) {
	if(index < 12) {
		return type_check_kinds[index];
	} else {
		return "(unknown)";
	}
}

static void	ubsan_location(struct source_location *l) {
	printf("%s:%u:%u: ", l->file, l->line, l->column);
}

void __ubsan_handle_add_overflow(struct overflow_data *data, uintptr_t lhs, uintptr_t rhs) {
	ubsan_location(&data->location);
	panic("add overflow");
}

void __ubsan_handle_sub_overflow(struct overflow_data *data, uintptr_t lhs, uintptr_t rhs) {
	ubsan_location(&data->location);
	panic("sub overflow");
}

void __ubsan_handle_pointer_overflow(struct overflow_data *data, uintptr_t lhs, uintptr_t rhs) {
	ubsan_location(&data->location);
	panic("pointer overflow\n");
}

void __ubsan_handle_mul_overflow(struct overflow_data *data, uintptr_t lhs, uintptr_t rhs) {
	ubsan_location(&data->location);
	panic("mul overflow");
}

void __ubsan_handle_divrem_overflow(struct overflow_data *data, uintptr_t lhs, uintptr_t rhs) {
	ubsan_location(&data->location);
	panic("divrem overflow");
}

void __ubsan_handle_negate_overflow(struct overflow_data *data, uintptr_t old) {
	ubsan_location(&data->location);
	panic("negate overflow");
}

void __ubsan_handle_shift_out_of_bounds(struct shift_out_of_bounds_data *data, uintptr_t lhs, uintptr_t rhs) {
	ubsan_location(&data->location);
	panic("shift out of bounds");
}

void __ubsan_handle_out_of_bounds(struct out_of_bounds_data *data, uintptr_t index) {
	ubsan_location(&data->location);
	panic("out of bounds");
}

void __ubsan_handle_type_mismatch_v1(struct type_mismatch_info *data, uintptr_t ptr) {
	ubsan_location(&data->location);

	if(data->alignment && (ptr & (data->alignment - 1)) != 0) {
		panic("unaligned access to %#lx (of type %s), which requires %lu-byte alignment", ptr, data->type->name, 1UL << data->alignment);
	} else {
		panic("%s %#lx (of type %s)", ubsan_type_check_kind(data->type_check_kind), ptr, data->type->name);
	}
}

void __ubsan_handle_invalid_builtin(struct invalid_builtin_data *data) {
	ubsan_location(&data->location);
	panic("invalid builtin");
}

void __ubsan_handle_load_invalid_value(struct invalid_value_data *data, uintptr_t val) {
	ubsan_location(&data->location);
	panic("load invalid value");
}

void __ubsan_handle_builtin_unreachable(struct unreachable_data *data) {
	ubsan_location(&data->location);
	panic("builtin unreachable");
}

void __ubsan_handle_nonnull_arg(struct nonnull_arg_data *data) {
	ubsan_location(&data->location);
	panic("nonnull arg");
}
