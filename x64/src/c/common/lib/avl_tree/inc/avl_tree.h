/* Copyright 2016 secXsQuared
 * Distributed under GPL license
 * See COPYING under root for details
 */

#ifndef _AVL_TREE_H_
#define _AVL_TREE_H_

#include "std_lib.h"

typedef struct _avl_tree_node_t
{
    struct _avl_tree_node_t *left;
    struct _avl_tree_node_t *right;
    struct _avl_tree_node_t *parent;
    int32_t height;
} avl_tree_node_t;

typedef struct
{
    avl_tree_node_t *root;
} avl_tree_t;


avl_tree_node_t *SAPI avl_tree_search(avl_tree_t *tree, avl_tree_node_t *entry,
                                       int32_t (*compare)(avl_tree_node_t *, avl_tree_node_t *));

void SAPI avl_tree_insert(avl_tree_t *tree, avl_tree_node_t *entry,
                          int32_t (*compare)(avl_tree_node_t *, avl_tree_node_t *));

void SAPI avl_tree_delete(avl_tree_t *tree, avl_tree_node_t *entry,
                          int32_t (*compare)(avl_tree_node_t *, avl_tree_node_t *));

void SAPI avl_tree_init(avl_tree_t *tree);

avl_tree_node_t *SAPI avl_tree_largest(avl_tree_t *tree);

avl_tree_node_t *SAPI avl_tree_smallest(avl_tree_t *tree);

avl_tree_node_t *SAPI avl_tree_larger(avl_tree_node_t *entry);

avl_tree_node_t *SAPI avl_tree_smaller(avl_tree_node_t *entry);

bool SAPI avl_tree_validate(avl_tree_t *tree, int32_t (*compare)(avl_tree_node_t *, avl_tree_node_t *));

int32_t SAPI avl_tree_size(avl_tree_t *tree);

#endif
