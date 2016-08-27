/* Copyright 2016 secXsQuared
 * Distributed under GPL license
 * See COPYING under root for details
 */

#ifndef _AVL_TREE_H_
#define _AVL_TREE_H_

#include "g_type.h"
#include "g_abi.h"
#include "k_stdlib.h"

typedef struct _avl_tree_node_t
{
    struct _avl_tree_node_t *left;
    struct _avl_tree_node_t *right;
    struct _avl_tree_node_t *parent;

    int32_t height;
} avl_tree_node_t;


/*
 * A comparison function between tree_node and your_node
 * Returns:
 * < 0 if tree_node < your_node
 * = 0 if tree_node == your_node
 * > 0 if tree_node > your_node
 */

typedef int32_t (*avl_tree_node_compare_func_t)(avl_tree_node_t* tree_node, avl_tree_node_t* your_node);

typedef struct
{
    avl_tree_node_compare_func_t compare;
    avl_tree_node_t *root;
} avl_tree_t;

avl_tree_node_t *KAPI avl_tree_search(avl_tree_t *tree, avl_tree_node_t *entry);

void KAPI avl_tree_insert(avl_tree_t *tree, avl_tree_node_t *entry);

avl_tree_node_t* KAPI avl_tree_delete(avl_tree_t *tree, avl_tree_node_t *entry);

void KAPI avl_tree_init(avl_tree_t *tree, avl_tree_node_compare_func_t);

avl_tree_node_t *KAPI avl_tree_largest(avl_tree_t *tree);

avl_tree_node_t *KAPI avl_tree_smallest(avl_tree_t *tree);

avl_tree_node_t *KAPI avl_tree_larger(avl_tree_node_t *entry);

avl_tree_node_t *KAPI avl_tree_smaller(avl_tree_node_t *entry);

bool KAPI avl_tree_validate(avl_tree_t *tree);

int32_t KAPI avl_tree_size(avl_tree_t *tree);

#endif
