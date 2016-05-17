#ifndef _HAL_MEM_H_
#define _HAL_MEM_H_

#include "../../sys/type.h"
#include "../../sys/kdef.h"

void SAPI mem_copy(void *src, void *dst, uint64_t size);

void SAPI mem_move(void *src, void *dst, uint64_t size);

void SAPI mem_set(void *src, int8_t const val, uint64_t size);

#endif