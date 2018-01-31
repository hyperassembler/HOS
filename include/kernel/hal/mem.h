#ifndef _KERNEL_HAL_MEM_H_
#define _KERNEL_HAL_MEM_H_

#include "type.h"
#include "lib/linked_list.h"

/**
 Kernel Memory Layout
**/
typedef uintptr_t physical_addr_t;
typedef uintptr_t virtual_addr_t;

#define KERNEL_PAGE_SIZE (0x1000ull)

#define KERNEL_AREA_START_VADDR (0xFFFF800000000000ull)
#define KERNEL_AREA_SIZE (0xFFFFFFFFFFFFFFFF - KERNEL_AREA_START_VADDR + 1)

#define KERNEL_PAGE_TABLE_VADDR (0xFFFFFF0000000000ull)
#define KERNEL_PAGE_TABLE_SIZE (0x8000000000ull)

// 510 GB
#define KERNEL_DYN_VADDR (KERNEL_PAGE_TABLE_VADDR + KERNEL_PAGE_TABLE_SIZE)
#define KERNEL_DYN_SIZE  (0x‬‭7F80000000ull)

#define KERNEL_HEAP_VADDR KERNEL_DYN_VADDR
#define KERNEL_INITIAL_HEAP_SIZE  (0x1000ull)

#define KERNEL_INITIAL_STACK_SIZE  (0x1000ull)
#define KERNEL_STACK_VADDR (KERNEL_DYN_VADDR + KERNEL_DYN_SIZE - KERNEL_INITIAL_STACK_SIZE)

// address space that is reserved for HAL to map its own stuff
#define KERNEL_HAL_VADDR  (KERNEL_DYN_VADDR + KERNEL_DYN_SIZE)
// 16MB Virtual Address Space
#define KERNEL_HAL_VADDR_LIMIT (0x1000000ull)

#define KERNEL_LOAD_VADDR (KERNEL_HAL_VADDR + KERNEL_HAL_VADDR_LIMIT)
#define KERNEL_LOAD_SIZE  (0xFFFFFFFFFFFFFFFF - KERNEL_LOAD_VADDR + 1)

/**
 * PMM init info
 */
typedef struct
{
    physical_addr_t base;
    uint64_t size;
    uint32_t attr;
} pmm_node_t;

typedef struct
{
    uint32_t num_of_nodes;
    pmm_node_t nodes[];
} pmm_info_t;


#endif
