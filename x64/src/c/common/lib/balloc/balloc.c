#include <balloc.h>
#include "linked_list.h"
#include "balloc.h"

typedef struct
{
    linked_list_node_t list_node;
} balloc_header_t;

static inline uint32_t _get_min_granularity()
{
     return sizeof(balloc_header_t);
}

static inline int32_t _order_of_two(uint32_t num)
{
    int32_t result = -1;
    if(num == 1)
    {
        result = 0;
    }
    else if(num % 2 == 0 && num != 0)
    {
        result = 1;
        while((num >> result) != 1)
        {
            result++;
        }
    }
    return result;
}

static inline int32_t _get_max_order(uint32_t size, uint32_t granularity)
{
    if(size == 0 || granularity == 0 || granularity > size || size % granularity != 0)
        return -1;
    return _order_of_two(size/granularity);
}

int32_t balloc_free_list_size(uint32_t size, uint32_t granularity)
{
    int32_t result = _get_max_order(size,granularity);
    return result == -1 ? (result) : (result) * ((int32_t)sizeof(linked_list_t));
}

int32_t balloc_bit_map_size(uint32_t size, uint32_t granularity)
{

}

int32_t balloc_init(balloc_desc_t* desc,
                    void* base,
                    uint32_t size,
                    uint32_t granularity)
{
    if( desc == NULL || base == NULL || granularity == 0 || size == 0 ||
            granularity < _get_min_granularity() ||
            size < granularity ||
            size % granularity != 0)
    {
        return 1;
    }

    desc->size = size;
    desc->base = base;
    desc->granularity = granularity;
    desc->bit_map = bit_map;
    desc->free_list = free_list;
}