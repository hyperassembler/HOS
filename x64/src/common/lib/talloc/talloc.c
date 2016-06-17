/* Copyright 2016 secXsQuared
 * Distributed under GPL license
 * See COPYING under root for details
 */

#include "linked_list.h"
#include "talloc.h"
#include "std_lib.h"

typedef struct
{
    avl_tree_node_t tree_node;
    uint64_t size;
    linked_list_t free_size_list;
} lalloc_free_list_node_t;

typedef struct
{
    avl_tree_node_t used_base_tree_node;
    linked_list_node_t free_size_list_node;
    avl_tree_node_t free_start_tree_node;
    avl_tree_node_t free_end_tree_node;
    void *base;
    uint64_t size;
    void *end;
} lalloc_desc_node_t;

static int32_t _lalloc_desc_node_size_compare(avl_tree_node_t *others, avl_tree_node_t *yours)
{
    uint64_t osize = OBTAIN_STRUCT_ADDR(others, free_size_tree_node, lalloc_desc_node_t)->size;
    uint64_t ysize = OBTAIN_STRUCT_ADDR(yours, free_size_tree_node, lalloc_desc_node_t)->size;
    if (osize == ysize)
        return 0;
    else if (osize > ysize)
        return 1;
    else
        return -1;
}

static int32_t _lalloc_desc_node_start_compare(avl_tree_node_t *others, avl_tree_node_t *yours)
{
    uint64_t osize = (uint64_t) (OBTAIN_STRUCT_ADDR(others, free_size_tree_node, lalloc_desc_node_t)->base);
    uint64_t ysize = (uint64_t) (OBTAIN_STRUCT_ADDR(yours, free_size_tree_node, lalloc_desc_node_t)->base);
    if (osize == ysize)
        return 0;
    else if (osize > ysize)
        return 1;
    else
        return -1;
}

static int32_t _lalloc_desc_node_end_compare(avl_tree_node_t *others, avl_tree_node_t *yours)
{
    uint64_t osize = (uint64_t) (OBTAIN_STRUCT_ADDR(others, free_size_tree_node, lalloc_desc_node_t)->end);
    uint64_t ysize = (uint64_t) (OBTAIN_STRUCT_ADDR(yours, free_size_tree_node, lalloc_desc_node_t)->end);
    if (osize == ysize)
        return 0;
    else if (osize > ysize)
        return 1;
    else
        return -1;
}

static void _lalloc_merge_nodes(lalloc_desc_node_t *node)
{

}

static int32_t _lalloc_node_alloc(talloc_desc_t *desc, lalloc_desc_node_t *node, uint64_t size, uint64_t alignment,
                                  void **out)
{
    if (desc == NULL || node == NULL || size == 0 || alignment == 0 || out == NULL)
    {
        return LALLOC_STATUS_INVALID_ARGUMENTS;
    }

    int32_t result = LALLOC_STATUS_NOT_ENOUGH_MEM;

    uint64_t real_base = align_up((uint64_t) node->base, alignment);
    uint64_t real_size = node->size - ((uint64_t) real_base - (uint64_t) node->base);

    if (real_size >= size)
    {
        // we've got a hit
        uint64_t prev_size = real_base - (uint64_t) node->base;
        uint64_t next_size = (uint64_t) node->base + node->size - real_base;

        if (prev_size == 0 && next_size == 0)
        {
            // if the block is just enough
            // detach the block and
            linked_list_remove_ref(&desc->free_list, &each_desc_node->list_node);
            desc->free();
        }

        result = LALLOC_STATUS_SUCCESS;
    }

    return result;
}

static inline _Bool _lalloc_is_desc_valid(talloc_desc_t *desc)
{
    return desc != NULL && desc->unlock != NULL && desc->lock != NULL && desc->alloc != NULL && desc->free != NULL;
}

int32_t lalloc(talloc_desc_t *desc,
               uint64_t size,
               uint64_t alignment,
               void **out)
{
    if (!_lalloc_is_desc_valid(desc) || size == 0 || alignment == 0 || out == NULL)
        return LALLOC_STATUS_INVALID_ARGUMENTS;

    int32_t result = LALLOC_STATUS_NOT_ENOUGH_MEM;


    desc->lock();


    linked_list_node_t *each_list_node = linked_list_first(&desc->free_list);
    while (each_list_node != NULL)
    {
        // calculate real base/size w.r.t. alignment
        lalloc_desc_node_t *each_desc_node = OBTAIN_STRUCT_ADDR(each_list_node, list_node, lalloc_desc_node_t);

        if (_lalloc_node_alloc(desc, each_desc_node, size, alignment, out) == LALLOC_STATUS_SUCCESS)
        {
            result = LALLOC_STATUS_SUCCESS;
            break;
        }

        each_list_node = linked_list_next(each_list_node);
    }


    desc->unlock();

    return result;
}

int32_t talloc_init(talloc_desc_t *desc,
                    void *base,
                    uint64_t size,
                    uint64_t page_size,
                    talloc_alloc_func alloc,
                    talloc_free_func free,
                    talloc_lock_func lock,
                    talloc_unlock_func unlock)
{
    if (desc == NULL || alloc == NULL || free == NULL ||
        size == 0 || page_size == 0 || base == NULL ||
        lock == NULL || unlock == NULL || (uint64_t) base % page_size != 0)
        return TALLOC_STATUS_INVALID_ARGUMENTS;

    avl_tree_init(&desc->used_base_tree);
    avl_tree_init(&desc->free_size_tree);
    avl_tree_init(&desc->free_start_tree);
    avl_tree_init(&desc->free_end_tree,);
    avl_tree_init(&desc->used_tree, _lalloc_desc_node_size_compare);

    desc->alloc = alloc;
    desc->free = free;
    desc->lock = lock;
    desc->unlock = unlock;
    desc->page_size = page_size;

    lalloc_desc_node_t *node = desc->alloc(sizeof(lalloc_desc_node_t));

    if (node == NULL)
        return LALLOC_STATUS_CANT_ALLOC_NODE;

    node->size = size;
    node->base = base;
    linked_list_push_back(&desc->free_list, &node->list_node);

    return LALLOC_STATUS_SUCCESS;
}

int32_t lalloc_mark_used(talloc_desc_t *desc,
                         void *base,
                         uint64_t size)
{

}

int32_t lalloc_mark_free(talloc_desc_t *desc,
                         void *base,
                         uint64_t size);

int32_t lfree(talloc_desc_t *desc,
              void *base);