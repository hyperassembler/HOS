#ifndef _S_VMM_H_
#define _S_VMM_H_
#include "g_abi.h"
#include "g_type.h"
#include "s_pmm.h"

typedef uint64_t k_virtual_addr_t;
typedef k_physical_addr_t k_address_space_t;

#define K_BASE_VADDR 0xFFFF800000000000
#define K_END_VADDR  0xFFFFFFFFFFFFFFFF

//U_BASE = 1MB
#define U_BASE_VADDR 0x0000000000100000
#define U_END_BADDR  0x7FFFFFFFFFFFFFFF

//
// all the address spaces passed by the kernel would be initialized by k_create_address_space
// which means the kernel area/ as well as the HAL reserved vaddr ranges would be properly mapped
//

typedef k_physical_addr_t (KAPI *k_physical_page_alloc)();

typedef void (KAPI *k_physical_page_free)(k_physical_addr_t page);

// this function should map the v_addr to p_addr for the target address space
extern void KAPI k_map_virtual_addr(k_physical_addr_t addr_space,
                                    k_virtual_addr_t v_addr,
                                    k_physical_addr_t p_addr,
                                    k_physical_page_alloc alloc);

typedef struct
{
    // the kernel always reserves this much virtual space for HAL
    // this is mainly used for recursive page tables or other HAL actions
    k_virtual_addr_t reserved_vaddr_base;
    k_virtual_addr_t reserved_vaddr_end;

    // the k_vaddr_alignment determines the alignment of the kernel's address space
    // the reserved virtual address spaces above is also subject to the alignment
    uint64_t k_vaddr_alignment;
} k_hal_vmm_info;

// this function always returns the physical address of the new address space
// the function should identity refer to the virtual address K_BASE_VADDR to K_END_VADDR according to the current address space
// so that these pages are global (modifying the mapping in this area affects everyone)
// the K_BASE_VADDR to K_END_VADDR includes the reserved virtual addr space by the HAL
// if HAL's reserved virtual addr will be mapped to different physical pages, the HAL should make the change
k_address_space_t KAPI k_create_address_space(k_address_space_t address_space,
                                              k_physical_page_alloc alloc);

// this function destroys the target address space without destroying the K_BASE_VADDR to K_END_VADDR
// target_addr_space is guaranteed to be not the same as the current address space
// when the function returns, the current address space must stay unchanged
void KAPI k_destroy_address_space(k_address_space_t address_space,
                                  k_physical_page_free free);

// as the name implies
void KAPI k_switch_address_space(k_address_space_t target_addr_space);

#endif