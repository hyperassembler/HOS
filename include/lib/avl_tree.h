#ifndef _LIB_AVL_TREE_H_
#define _LIB_AVL_TREE_H_

#include "type.h"
#include "lib/sxtdlib.h"

typedef struct _k_avl_tree_node_t
{
	struct _k_avl_tree_node_t *left;
	struct _k_avl_tree_node_t *right;
	struct _k_avl_tree_node_t *parent;

	int32_t height;
} avl_tree_node_t;

/*
* A comparison function between tree_node and your_node
* Returns:
* < 0 if tree_node < your_node
* = 0 if tree_node == your_node
* > 0 if tree_node > your_node
*/
typedef struct _k_avl_tree_t
{
	callback_func_t compare;
	avl_tree_node_t *root;
} avl_tree_t;

avl_tree_node_t *KABI lb_avl_tree_search(avl_tree_t *tree, avl_tree_node_t *entry);

void KABI lb_avl_tree_insert(avl_tree_t *tree, avl_tree_node_t *entry);

avl_tree_node_t *KABI lb_avl_tree_delete(avl_tree_t *tree, avl_tree_node_t *entry);

void KABI lb_avl_tree_init(avl_tree_t *tree, callback_func_t compare);

avl_tree_node_t *KABI lb_avl_tree_largest(avl_tree_t *tree);

avl_tree_node_t *KABI lb_avl_tree_smallest(avl_tree_t *tree);

avl_tree_node_t *KABI lb_avl_tree_larger(avl_tree_node_t *entry);

avl_tree_node_t *KABI lb_avl_tree_smaller(avl_tree_node_t *entry);

bool KABI lb_avl_tree_validate(avl_tree_t *tree);

int32_t KABI lb_avl_tree_size(avl_tree_t *tree);

#endif
