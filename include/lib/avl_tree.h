#ifndef LIB_AVL_TREE_H
#define LIB_AVL_TREE_H

#include "type.h"
#include "lib/sxtdlib.h"

struct avl_tree_node
{
	struct avl_tree_node *left;
	struct avl_tree_node *right;
	struct avl_tree_node *parent;

	int32 height;
};

typedef int32 (SXAPI *avl_tree_compare_func)(struct avl_tree_node *left, struct avl_tree_node *right);

/*
* A comparison function between tree_node and your_node
* Returns:
* < 0 if tree_node < your_node
* = 0 if tree_node == your_node
* > 0 if tree_node > your_node
*/
struct avl_tree
{
	avl_tree_compare_func compare;
	struct avl_tree_node *root;
};

struct avl_tree_node *SXAPI lb_avl_tree_search(struct avl_tree *tree, struct avl_tree_node *entry);

void SXAPI lb_avl_tree_insert(struct avl_tree *tree, struct avl_tree_node *entry);

struct avl_tree_node *SXAPI lb_avl_tree_delete(struct avl_tree *tree, struct avl_tree_node *entry);

void SXAPI lb_avl_tree_init(struct avl_tree *tree, avl_tree_compare_func compare);

struct avl_tree_node *SXAPI lb_avl_tree_largest(struct avl_tree *tree);

struct avl_tree_node *SXAPI lb_avl_tree_smallest(struct avl_tree *tree);

struct avl_tree_node *SXAPI lb_avl_tree_larger(struct avl_tree_node *entry);

struct avl_tree_node *SXAPI lb_avl_tree_smaller(struct avl_tree_node *entry);

bool SXAPI lb_avl_tree_validate(struct avl_tree *tree);

int32 SXAPI lb_avl_tree_size(struct avl_tree *tree);

#endif
