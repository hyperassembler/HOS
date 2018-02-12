#ifndef _KERNEL_HAL_MEM_H_
#define _KERNEL_HAL_MEM_H_

#include "type.h"
#include "lib/linked_list.h"
#include "kernel/hal/memdef.h"

/**
 * From linker.inc
 */
extern char KERNEL_IMAGE_END_VADDR[];

/**
 * PMM init info
 */
typedef struct
{
    uintptr_t base;
    uint64_t size;
    uint32_t attr;
} pmm_node_t;

typedef struct
{
    uint32_t num_of_nodes;
    pmm_node_t nodes[];
} pmm_info_t;

#endif
