#ifndef _S_VMM_H_
#define _S_VMM_H_

#include "g_abi.h"
#include "g_type.h"
#include "s_pmm.h"

typedef uint64_t virtual_addr_t;
typedef physical_addr_t address_space_t;

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

//
// all the address spaces passed by the kernel would be initialized by k_create_address_space
// which means the kernel area/ as well as the HAL reserved vaddr ranges would be properly mapped
//

typedef physical_addr_t (KABI *page_alloc_func_t)();

typedef void (KABI *page_free_func_t)(physical_addr_t page);

#define K_PAGE_ATTR_KERNEL (1 << 2)
#define K_PAGE_ATTR_CACHED (1 << 3)
#define K_PAGE_ATTR_NOT_EXECUTABLE (1 << 63)
#define K_PAGE_ATTR_WRITABLE (1 << 1)

// this function should map the v_addr to p_addr for the target address space
extern void KABI mm_map_virtual_addr(physical_addr_t addr_space,
                                     virtual_addr_t v_addr,
                                     physical_addr_t p_addr,
                                     uint64_t attribute,
                                     page_alloc_func_t alloc);

typedef struct
{
    // the kernel always reserves this much virtual space for HAL
    // this is mainly used for recursive page tables or other HAL actions
    virtual_addr_t reserved_vaddr_base;
    virtual_addr_t reserved_vaddr_end;

    // the k_vaddr_alignment determines the alignment of the kernel's address space
    // the reserved virtual address spaces above is also subject to the alignment
    uint64_t k_vaddr_alignment;
} k_hal_vmm_info;

// this function always returns the physical address of the new address space
// the function should identity refer to the virtual address K_BASE_VADDR to K_END_VADDR according to the current address space
// so that these pages are global (modifying the mapping in this area affects everyone)
// the K_BASE_VADDR to K_END_VADDR includes the reserved virtual addr space by the HAL
// if HAL's reserved virtual addr will be mapped to different physical pages, the HAL should make the change
address_space_t KABI mm_create_address_space(address_space_t address_space,
                                             page_alloc_func_t alloc);

// this function destroys the target address space without destroying the K_BASE_VADDR to K_END_VADDR
// target_addr_space is guaranteed to be not the same as the current address space
// when the function returns, the current address space must stay unchanged
void KABI mm_destroy_address_space(address_space_t address_space,
                                   page_free_func_t free);

// as the name implies
void KABI mm_switch_address_space(address_space_t target_addr_space);

#endif