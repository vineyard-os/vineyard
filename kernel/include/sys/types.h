#pragma once

#include <stdint.h>

#define __SSIZE_TYPE__ __typeof__(_Generic((__SIZE_TYPE__)0,		\
                      unsigned long long int : (long long int)0,	\
                      unsigned long int : (long int)0,				\
                      unsigned int : (int)0,						\
                      unsigned short : (short)0,					\
                      unsigned char : (signed char)0))

typedef __SSIZE_TYPE__ ssize_t;

typedef intptr_t off_t;
typedef intptr_t off64_t;
