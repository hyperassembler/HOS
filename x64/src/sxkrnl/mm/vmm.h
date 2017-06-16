#ifndef _K_VMM_H_
#define _K_VMM_H_

#include "../arch/amd64/mem.h"
#include "avl_tree.h"
#include "atomic.h"
#include "spin_lock.h"

#define VMM_STATUS_SUCCESS 0
#define VMM_STATUS_INVALID_ARGUMENTS 1
#define VMM_STATUS_CANNOT_ALLOC_NODE 2

typedef struct
{
    uint32_t attr;
} virtual_addr_attribute_t;

typedef struct
{
    avl_tree_t region_tree;
    _Bool initialized;
    k_spin_lock_t lock;
} vmm_descriptor_t;

int32_t KABI mm_vmm_init(vmm_descriptor_t *desc);

int32_t KABI mm_alloc_virtual_address(vmm_descriptor_t *desc,
                                      virtual_addr_t base,
                                      uint64_t size,
                                      virtual_addr_attribute_t attr);

int64_t KABI mm_query_virtual_address(vmm_descriptor_t *desc, virtual_addr_t v_addr, uint64_t *out);

int64_t KABI mm_free_virtual_address(vmm_descriptor_t *desc, virtual_addr_t base);

#endif
