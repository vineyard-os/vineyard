#pragma once

#include <stdint.h>

struct source_location {
    const char *file;
    uint32_t line;
    uint32_t column;
};

struct type_descriptor {
    uint16_t kind;
    uint16_t info;
    char name[];
};

struct overflow_data {
    struct source_location location;
    struct type_descriptor *type;
};

struct shift_out_of_bounds_data {
    struct source_location location;
    struct type_descriptor *lhs_type;
    struct type_descriptor *rhs_type;
};

struct out_of_bounds_data {
    struct source_location location;
    struct type_descriptor *array_type;
    struct type_descriptor *index_type;
};

struct non_null_return_data {
    struct source_location location;
};

struct type_mismatch_v1 {
    struct source_location location;
    struct type_descriptor *type;
    uint8_t alignment;
    uint8_t type_check_kind;
};

struct vla_bound_data {
    struct source_location location;
    struct type_descriptor *type;
};

struct invalid_value_data {
	struct source_location location;
	unsigned char kind;
};

struct invalid_builtin_data {
    struct source_location location;
    struct type_descriptor *type;
};

struct unreachable_data {
    struct source_location location;
};

struct nonnull_arg_data {
    struct source_location location;
};

void __ubsan_handle_add_overflow(struct overflow_data *data, uintptr_t lhs, uintptr_t rhs);
void __ubsan_handle_sub_overflow(struct overflow_data *data, uintptr_t lhs, uintptr_t rhs);
void __ubsan_handle_pointer_overflow(struct overflow_data *data, uintptr_t lhs, uintptr_t rhs);
void __ubsan_handle_mul_overflow(struct overflow_data *data, uintptr_t lhs, uintptr_t rhs);
void __ubsan_handle_divrem_overflow(struct overflow_data *data, uintptr_t lhs, uintptr_t rhs);
void __ubsan_handle_negate_overflow(struct overflow_data *data, uintptr_t old);
void __ubsan_handle_shift_out_of_bounds(struct shift_out_of_bounds_data *data, uintptr_t lhs, uintptr_t rhs);
void __ubsan_handle_out_of_bounds(struct out_of_bounds_data *data, uintptr_t index);
void __ubsan_handle_type_mismatch_v1(struct type_mismatch_v1 *data, uintptr_t ptr);
void __ubsan_handle_invalid_builtin(struct invalid_builtin_data *data);
void __ubsan_handle_load_invalid_value(struct invalid_value_data *data, uintptr_t val);
void __ubsan_handle_builtin_unreachable(struct unreachable_data *data);
void __ubsan_handle_nonnull_arg(struct nonnull_arg_data *data);
