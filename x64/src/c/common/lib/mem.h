#ifndef _HAL_MEM_H_
#define _HAL_MEM_H_

#include "../sys/type.h"
#include "../sys/kdef.h"

void _KERNEL_ABI mem_copy(void *src, void *dst, uint64_t size);

void _KERNEL_ABI mem_move(void *src, void *dst, uint64_t size);

void _KERNEL_ABI mem_set(void *src, int8_t const val, uint64_t size);

#endif