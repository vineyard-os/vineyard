#pragma once

#include <boot/panic.h>
#include <stddef.h>

#define ABORT panic("dlmalloc called abort()")
#define ABORT_ON_ASSERT_FAILURE 0

#define LACKS_ERRNO_H
#define LACKS_TIME_H
#define LACKS_STRINGS_H
#define LACKS_SYS_PARAM_H
#define LACKS_FCNTL_H
#define LACKS_UNISTD_H
#define LACKS_SCHED_H

#define NO_MALLOC_STATS 1
#define MALLOC_FAILURE_ACTION

#define HAVE_MORECORE 0
#define HAVE_MMAP 1

#define USE_LOCKS 0
#define USE_SPIN_LOCKS 0

#define MALLOC_FAILURE_ACTION

#define HAVE_MORECORE 0
#define HAVE_MMAP     1
#define HAVE_MREMAP   0
#define MMAP_CLEARS   0

#define malloc_getpagesize 0x1000

#define ENOMEM 1
#define EINVAL 2

#pragma clang diagnostic ignored "-Wstrict-prototypes"
#pragma clang diagnostic ignored "-Wsign-conversion"
#pragma clang diagnostic ignored "-Wnull-pointer-arithmetic"
#pragma clang diagnostic ignored "-Wmissing-prototypes"
#pragma clang diagnostic ignored "-Wshorten-64-to-32"
// #pragma clang diagnostic ignored "-W"
// #pragma clang diagnostic ignored "-W"
#include <mm/dlmalloc.h>
