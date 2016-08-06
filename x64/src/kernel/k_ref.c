#include "k_ref.h"

typedef struct
{
    avl_tree_node_t tree_node;
    void* ref_ptr;
    uint32_t ref_count;
    k_ref_callback_func_t callback;
} k_ref_node_t;

/*
 * A comparison function between tree_node and your_node
 * Returns:
 * < 0 if tree_node < your_node
 * = 0 if tree_node == your_node
 * > 0 if tree_node > your_node
 */
static int32_t _avl_compare(avl_tree_node_t *tree_node, avl_tree_node_t *my_node)
{
    k_physical_addr_t tree_base = OBTAIN_STRUCT_ADDR(tree_node,
                                                     k_physical_page_descriptor_t,
                                                     avl_tree_node)->base;
    k_physical_addr_t my_base = OBTAIN_STRUCT_ADDR(my_node,
                                                   k_physical_page_descriptor_t,
                                                   avl_tree_node)->base;
    if (tree_base > my_base)
        return 1;
    else if (tree_base < my_base)
        return -1;
    else
        return 0;
}

int32_t KAPI k_ref_init(k_ref_desc_t *desc)
{
    if(desc == NULL)
        return K_REF_STATUS_INVALID_ARGUMENTS;

    avl_tree_init(&desc->avl_tree);
}



int32_t KAPI k_ref_create(void *ptr, k_ref_callback_func_t callback, void *context)
{
    return 0;
}

int32_t KAPI k_ref_inc(void *ptr)
{
    return 0;
}

int32_t KAPI k_ref_dec(void *ptr)
{
    return 0;
}
