#pragma once

#include <kern/cdef.h>

struct atree_node
{
    struct atree_node *left;
    struct atree_node *right;
    int32 height;
};

/*
* A comparison function between self (yours) and treenode (tree's)
* Returns:
* < 0 if treenode < self
* = 0 if treenode = self
* > 0 if treenode > self
*/
typedef int32 (*atree_cmp_fn)(struct atree_node *tree_node, struct atree_node *self);

struct atree
{
    atree_cmp_fn cmpf;
    struct atree_node *root;
};


/*
 * init operations
 */

void
atree_init(struct atree *tree, atree_cmp_fn compare);

static inline bool
atree_empty(struct atree *tree)
{
    return (tree->root == NULL);
}


/*
 * tree operations
 */

struct atree_node *
atree_search(struct atree *tree, struct atree_node *entry);


struct atree_node *
atree_insert(struct atree *tree, struct atree_node *entry);


struct atree_node *
atree_remove(struct atree *tree, struct atree_node *entry);


/*
 * traversal operations
 */

struct atree_node *
atree_max(struct atree *tree);

struct atree_node *
atree_min(struct atree *tree);

struct atree_node *
atree_next(struct atree *tree, struct atree_node *entry);

struct atree_node *
atree_prev(struct atree *tree, struct atree_node *entry);


/*
 * internal operations (testing only)
 */

bool
atree_validate(struct atree *tree);

uint32
atree_size(struct atree *tree);
