#include "alloc.h"
#include "vmm.h"

typedef struct
{
    avl_tree_node_t tree_node;
    virtual_addr_attribute_t attribute;
    virtual_addr_t base;
    uint64_t size;
} virtual_addr_descriptor_t;

/*
 * A comparison function between tree_node and your_node
 * Returns:
 * < 0 if tree_node < your_node
 * = 0 if tree_node == your_node
 * > 0 if tree_node > your_node
 */
static int32_t mmp_base_addr_compare(void *tree_node, void *my_node)
{
    virtual_addr_descriptor_t *that = OBTAIN_STRUCT_ADDR(tree_node,
                                                           virtual_addr_descriptor_t,
                                                           tree_node);
    virtual_addr_descriptor_t *mine = OBTAIN_STRUCT_ADDR(my_node,
                                                           virtual_addr_descriptor_t,
                                                           tree_node);

    // if overlap, consider them to be the same
    if (lb_is_overlap(that->base, that->base + that->size, mine->base, mine->base + mine->size) == 1)
        return 0;
    else if (that->base < mine->base)
        return -1;
    else
        return 1;
}

int32_t KABI mm_vmm_init(vmm_descriptor_t *desc)
{
    if (desc == NULL || desc->initialized)
    {
        return VMM_STATUS_INVALID_ARGUMENTS;
    }
    lb_avl_tree_init(&desc->region_tree, mmp_base_addr_compare);

    return VMM_STATUS_SUCCESS;
}

int32_t KABI mm_alloc_virtual_address(vmm_descriptor_t *desc,
                                      virtual_addr_t base,
                                      uint64_t size,
                                      virtual_addr_attribute_t attr)
{
    if(desc == NULL || !desc->initialized)
    {
        return VMM_STATUS_INVALID_ARGUMENTS;
    }
    virtual_addr_descriptor_t* node = ke_alloc(sizeof(virtual_addr_descriptor_t));

    if(node == NULL)
    {
        return VMM_STATUS_CANNOT_ALLOC_NODE;
    }

    node->base =base;
    node->size = size;
    node->attribute = attr;

    lb_avl_tree_insert(&desc->region_tree, &node->tree_node);

    return VMM_STATUS_SUCCESS;
}

int64_t KABI mm_query_virtual_address(vmm_descriptor_t *desc, virtual_addr_t v_addr, uint64_t *out)
{
    UNREFERENCED(desc);
    UNREFERENCED(v_addr);
    UNREFERENCED(out);
    return 0;
}

int64_t KABI mm_free_virtual_address(vmm_descriptor_t *desc, virtual_addr_t base)
{
    UNREFERENCED(desc);
    UNREFERENCED(base);
    return 0;
}