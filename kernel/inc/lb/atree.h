#pragma once

#include "cdef.h"


struct atree_node
{
    struct atree_node *left;
    struct atree_node *right;
    int32 height;
};

/**
* A comparison function between self (yours) and treenode (tree's)
* Returns:
* < 0 if treenode < self
* = 0 if treenode = self
* > 0 if treenode > self
*/
typedef int32 (*atree_cmp_fp)(struct atree_node *tree_node, struct atree_node *self);

struct a_tree
{
    atree_cmp_fp cmpf;
    struct atree_node *root;
};


struct atree_node *
lb_atree_search(struct a_tree *tree, struct atree_node *entry);


struct atree_node *
lb_atree_insert(struct a_tree *tree, struct atree_node *entry);


struct atree_node *
lb_atree_delete(struct a_tree *tree, struct atree_node *entry);


void
lb_atree_init(struct a_tree *tree, atree_cmp_fp compare);


struct atree_node *
lb_atree_max(struct a_tree *tree);


struct atree_node *
lb_atree_min(struct a_tree *tree);


struct atree_node *
lb_atree_next(struct a_tree *tree, struct atree_node *entry);


struct atree_node *
lb_atree_prev(struct a_tree *tree, struct atree_node *entry);

bool
lb_atree_validate(struct a_tree *tree);

uint32
lb_atree_size(struct a_tree *tree);

