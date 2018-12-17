#pragma once

#include <stdint.h>

#define vy_unused __attribute__((unused))
#define vy_pure __attribute__((pure))
#define vy_packed __attribute__((packed))

typedef uintptr_t vy_status_t;

#define VY_OK 0UL
#define VY_ERR 1UL
