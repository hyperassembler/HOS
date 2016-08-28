#include "k_alloc.h"
#include "k_vmm.h"

typedef struct
{
    k_avl_tree_node_t tree_node;
    k_virtual_addr_attribute_t attribute;
    k_virtual_addr_t base;
    uint64_t size;
} k_virtual_addr_descriptor_t;

/*
 * A comparison function between tree_node and your_node
 * Returns:
 * < 0 if tree_node < your_node
 * = 0 if tree_node == your_node
 * > 0 if tree_node > your_node
 */
static int32_t _avl_compare(k_avl_tree_node_t *tree_node, k_avl_tree_node_t *my_node)
{
    k_virtual_addr_descriptor_t *that = OBTAIN_STRUCT_ADDR(tree_node,
                                                           k_virtual_addr_descriptor_t,
                                                           tree_node);
    k_virtual_addr_descriptor_t *mine = OBTAIN_STRUCT_ADDR(my_node,
                                                           k_virtual_addr_descriptor_t,
                                                           tree_node);

    // if overlap, consider them to be the same
    if (ke_is_overlap(that->base, that->base + that->size, mine->base, mine->base + mine->size) == 1)
        return 0;
    else if (that->base < mine->base)
        return -1;
    else
        return 1;
}

int32_t KAPI k_vmm_init(k_vmm_descriptor_t *desc)
{
    if (desc == NULL || desc->initialized)
    {
        return VMM_STATUS_INVALID_ARGUMENTS;
    }
    ke_avl_tree_init(&desc->region_tree, _avl_compare);

    return VMM_STATUS_SUCCESS;
}

int32_t KAPI k_alloc_virtual_address(k_vmm_descriptor_t *desc,
                                k_virtual_addr_t base,
                                uint64_t size,
                                k_virtual_addr_attribute_t attr)
{
    if(desc == NULL || !desc->initialized)
    {
        return VMM_STATUS_INVALID_ARGUMENTS;
    }
    k_virtual_addr_descriptor_t* node = k_alloc(sizeof(k_virtual_addr_descriptor_t));

    if(node == NULL)
    {
        return VMM_STATUS_CANNOT_ALLOC_NODE;
    }

    node->base =base;
    node->size = size;
    node->attribute = attr;

    ke_avl_tree_insert(&desc->region_tree, &node->tree_node);

    return VMM_STATUS_SUCCESS;
}

int64_t KAPI k_query_virtual_address(k_vmm_descriptor_t *desc, k_virtual_addr_t v_addr, uint64_t *out)
{
    return 0;
}

int64_t KAPI k_free_virtual_address(k_vmm_descriptor_t *desc, k_virtual_addr_t base)
{
    return 0;
}