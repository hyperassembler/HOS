#ifndef _S_MEM_H_
#define _S_MEM_H_
#include "s_def.h"

#define K_PAGE_SIZE 4096

typedef struct
{
    // the kernel always reserves this much virtual space from the highest vaddr
    // this is mainly used for recursive page tables
    uint64_t reserved_v_addr_space;
} k_hal_vmm_info;

typedef void*(*k_map_virtual_addr_alloc)();
typedef uint64_t k_address_space_t;

// the alloc function returns the physical address of a page
// NULL = no available physical page
void k_map_virtual_addr(k_address_space_t addr_space, uint64_t v_addr, uint64_t p_addr, k_map_virtual_addr_alloc alloc);

// this function always returns the physical address (x86) or whatever makes sense to the HAL
// the whole virtual address should be unmapped except for the reserved virtual space.
// HAL maps it however it wants
k_address_space_t k_create_virtual_addr_space();
void k_destroy_virtual_addr_space(k_address_space_t addr_space);

// this function gives the context of a process and the HAL returns its address space
k_address_space_t k_get_addr_space(void* context);


#endif