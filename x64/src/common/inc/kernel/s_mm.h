#ifndef _S_PMM_H_
#define _S_PMM_H_

#include "s_abi.h"
#include "s_type.h"

#define K_PAGE_SIZE 4096

typedef uint64_t k_virtual_addr_t;
typedef uint64_t k_physical_addr_t;

//
// all the address spaces passed by the kernel would be initialized by k_create_address_space
// which means the kernel area/ as well as the HAL reserved vaddr ranges would be properly mapped
//

typedef k_physical_addr_t (KAPI *k_physical_page_alloc)(k_virtual_addr_t virtual_addr);

typedef void (KAPI *k_physical_page_free)(void *v_addr, k_physical_addr_t page);


// this function should map the v_addr to p_addr for the target address space
extern void KAPI k_map_virtual_addr(k_physical_addr_t addr_space,
                                    k_virtual_addr_t v_addr,
                                    k_physical_addr_t p_addr,
                                    k_physical_page_alloc alloc);
#endif