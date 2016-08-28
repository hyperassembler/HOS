/* Copyright 2016 secXsQuared
 * Distributed under GPL license
 * See COPYING under root for details
 */

#ifndef _K_AVL_TREE_H_
#define _K_AVL_TREE_H_

#include "g_type.h"
#include "g_abi.h"
#include "k_stdlib.h"

typedef struct _k_avl_tree_node_t
{
    struct _k_avl_tree_node_t *left;
    struct _k_avl_tree_node_t *right;
    struct _k_avl_tree_node_t *parent;

    int32_t height;
} k_avl_tree_node_t;

/*
* A comparison function between tree_node and your_node
* Returns:
* < 0 if tree_node < your_node
* = 0 if tree_node == your_node
* > 0 if tree_node > your_node
*/
typedef struct _k_avl_tree_t
{
    k_callback_func_t compare;
    k_avl_tree_node_t *root;
} k_avl_tree_t;

k_avl_tree_node_t *KAPI ke_avl_tree_search(k_avl_tree_t *tree, k_avl_tree_node_t *entry);

void KAPI ke_avl_tree_insert(k_avl_tree_t *tree, k_avl_tree_node_t *entry);

k_avl_tree_node_t *KAPI ke_avl_tree_delete(k_avl_tree_t *tree, k_avl_tree_node_t *entry);

void KAPI ke_avl_tree_init(k_avl_tree_t *tree, k_callback_func_t compare);

k_avl_tree_node_t *KAPI ke_avl_tree_largest(k_avl_tree_t *tree);

k_avl_tree_node_t *KAPI ke_avl_tree_smallest(k_avl_tree_t *tree);

k_avl_tree_node_t *KAPI ke_avl_tree_larger(k_avl_tree_node_t *entry);

k_avl_tree_node_t *KAPI ke_avl_tree_smaller(k_avl_tree_node_t *entry);

bool KAPI ke_avl_tree_validate(k_avl_tree_t *tree);

int32_t KAPI ke_avl_tree_size(k_avl_tree_t *tree);

#endif
