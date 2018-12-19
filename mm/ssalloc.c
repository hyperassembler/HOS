#include "kern/cdef.h"
#include "kern/mlayout.h"
#include "lb/dlist.h"

/**
 * Simplified Slab Allocator
 */

#define LARGE_THRESHOLD (8)

struct ssalloc_page_desc
{
    struct dlist_node node;
    uint32 used_obj_num;
    uint8 free_list[0]; /* free list inside each slab */
};

struct ssalloc_obj_desc
{
    struct dlist free_list;
    struct dlist full_list;
    struct dlist empty_list;
    usize obj_size;
    uint32 align;
};

struct ssalloc_desc
{
    uint32 chunk_size;
    void* malloc_chunk(void);   // allocate a chunk
    void* free_chunk(void);     // free a chunk
};

static void* scalloc_large(struct ssalloc_desc* desc, struct ssalloc_obj_desc* obj_desc)
{

}

static void scfree_large(struct ssalloc_desc* desc, struct ssalloc_obj_desc* obj_desc, void* addr)
{

}

static void* scalloc_small(struct ssalloc_desc* desc, struct ssalloc_obj_desc* obj_desc)
{
    // check the current free list first
    if (lb_dlist_size(&obj_desc->free_list) > 0)
    {
        // if it exists then we grab something from the list
        struct llist_node* node = lb_dlist_first(node);
    }
    else
    {

    }
}

static void scfree_small(struct ssalloc_desc* desc, struct ssalloc_obj_desc* obj_desc, void* addr)
{

}

void ssalloc_desc_init(struct ssalloc_desc* desc)
{

}

void ssalloc_obj_desc_init(struct ssalloc_obj_desc* obj_desc, usize obj_size, uint32 align)
{
    obj_desc->obj_size = obj_size;
    obj_desc->align = align;
    lb_dlist_init(&obj_desc->empty_list);
    lb_dlist_init(&obj_desc->full_list);
    lb_dlist_init(&obj_desc->free_list);
}

void* scalloc(struct ssalloc_desc* desc, struct ssalloc_obj_desc* obj_desc)
{
    void* ret = NULL;
    if(obj_desc->obj_size < desc->chunk_size / LARGE_THRESHOLD)
    {
        ret = scalloc_small(desc, obj_desc);
    }
    else
    {
        // large objects
        ret = scalloc_large(desc, obj_desc);
    }

    return ret;
}

void scfree(struct ssalloc_desc* desc, struct ssalloc_obj_desc* obj_desc, void* address)
{
    if(obj_desc->obj_size < desc->chunk_size)
    {
        scfree_small(desc, obj_desc, address);
    }
    else
    {
        // large objects
        scfree_large(desc, obj_desc, address);
    }
}