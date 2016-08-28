#ifndef _K_VMM_H_
#define _K_VMM_H_

#include "s_vmm.h"
#include "k_avl_tree.h"
#include "k_atomic.h"

#define VMM_STATUS_SUCCESS 0
#define VMM_STATUS_INVALID_ARGUMENTS 1
#define VMM_STATUS_CANNOT_ALLOC_NODE 2

typedef struct
{
    uint32_t attr;
} k_virtual_addr_attribute_t;

typedef struct
{
    k_avl_tree_t region_tree;
    _Bool initialized;
    k_spin_lock_t lock;
} k_vmm_descriptor_t;

int32_t k_vmm_init(k_vmm_descriptor_t *desc);

int32_t k_alloc_virtual_address(k_vmm_descriptor_t *desc,
                                k_virtual_addr_t base,
                                uint64_t size,
                                k_virtual_addr_attribute_t attr);

int64_t k_query_virtual_address(k_vmm_descriptor_t* desc, k_virtual_addr_t v_addr, uint64_t* out);

int64_t k_free_virtual_address(k_vmm_descriptor_t *desc, k_virtual_addr_t base);

#endif