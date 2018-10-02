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

struct atree
{
    atree_cmp_fp cmpf;
    struct atree_node *root;
};


struct atree_node *
lb_atree_search(struct atree *tree, struct atree_node *entry);


struct atree_node *
lb_atree_insert(struct atree *tree, struct atree_node *entry);


struct atree_node *
lb_atree_delete(struct atree *tree, struct atree_node *entry);


void
lb_atree_init(struct atree *tree, atree_cmp_fp compare);


struct atree_node *
lb_atree_max(struct atree *tree);


struct atree_node *
lb_atree_min(struct atree *tree);


struct atree_node *
lb_atree_next(struct atree *tree, struct atree_node *entry);


struct atree_node *
lb_atree_prev(struct atree *tree, struct atree_node *entry);

bool
lb_atree_validate(struct atree *tree);

uint32
lb_atree_size(struct atree *tree);

