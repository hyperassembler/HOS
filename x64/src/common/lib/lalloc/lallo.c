#include <lalloc.h>
#include "lalloc.h"

typedef struct
{
    linked_list_node_t list_node;
    void *base;
    uint64_t size;
} lalloc_desc_node_t;

static void _lalloc_merge_nodes(lalloc_desc_node_t* node)
{

}

int32_t lalloc_aligned(lalloc_desc_t* desc,
                       uint64_t size,
                       uint64_t alignment,
                       void** out)
{
    if(desc == NULL || size == 0 || alignment == 0 || out == NULL)
        return LALLOC_STATUS_INVALID_ARGUMENTS;
    linked_list_node_t* each_list_node = linked_list_first(&desc->free_list);
    while(each_list_node != NULL)
    {
        
        each_list_node = linked_list_next(each_list_node);
    }
}

int32_t lalloc_set_alloc(lalloc_desc_t* desc, lalloc_alloc_func alloc)
{
    if(alloc != NULL && desc != NULL)
    {
        desc->alloc = alloc;
        return LALLOC_STATUS_SUCCESS;
    }
    else
        return LALLOC_STATUS_INVALID_ARGUMENTS;
}

int32_t lalloc_set_free(lalloc_desc_t* desc, lalloc_free_func free)
{
    if(free != NULL && desc != NULL)
    {
        desc->free = free;
        return LALLOC_STATUS_SUCCESS;
    }
    else
        return LALLOC_STATUS_INVALID_ARGUMENTS;
}

int32_t lalloc(lalloc_desc_t* desc,
               uint64_t size,
               void** out)
{
    return lalloc_aligned(desc, size, 1, out);
}

int32_t lalloc_init(lalloc_desc_t *desc,
                    void *base,
                    uint64_t size,
                    lalloc_alloc_func alloc,
                    lalloc_free_func free)
{
    if (desc == NULL || alloc == NULL || free == NULL || base == NULL)
        return LALLOC_STATUS_INVALID_ARGUMENTS;

    linked_list_init(&desc->free_list);
    linked_list_init(&desc->used_list);
    desc->alloc = alloc;
    desc->free = free;

    lalloc_desc_node_t *node = desc->alloc(sizeof(lalloc_desc_node_t));

    if (node == NULL)
        return LALLOC_STATUS_CANT_ALLOC_NODE;

    node->size = size;
    node->base = base;
    linked_list_push_back(&desc->free_list, &node->list_node);

    return LALLOC_STATUS_SUCCESS;
}

int32_t lalloc_mark_used(lalloc_desc_t *desc,
                         void *base,
                         uint64_t size);

int32_t lalloc_mark_free(lalloc_desc_t* desc,
                         void *base,
                         uint64_t size);

int32_t lfree(lalloc_desc_t *desc,
              void *base);