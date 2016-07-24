#ifndef _K_REF_H_
#define _K_REF_H_

#include "avl_tree.h"

typedef struct
{
    avl_tree_node_t* tree_node;
    void* ref_ptr;
    uint32_t ref_count;
} k_ref_;

void k_ref();

#endif