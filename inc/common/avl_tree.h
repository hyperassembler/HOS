#pragma once

#include <common/cdef.h>

struct avl_node
{
    struct avl_node *left;
    struct avl_node *right;
    int height;
};

/*
* A comparison function between self (yours) and other
* Returns:
* < 0 if other < self
* = 0 if other = self
* > 0 if other > self
*/
typedef int (*avl_cmpf)(struct avl_node *tree_node, struct avl_node *self);

struct avl_root
{
    avl_cmpf cmpf;
    struct avl_node *root;
};


/*
 * init operations
 */

static inline void
avl_init(struct avl_root *root, avl_cmpf cmpf)
{
    root->root = NULL;
    root->cmpf = cmpf;
}

static inline int
avl_empty(struct avl_root *root)
{
    return (root->root == NULL);
}

/*
 * tree operations
 */

struct avl_node *
avl_search(struct avl_root *root, struct avl_node *node);


struct avl_node *
avl_insert(struct avl_root *root, struct avl_node *node);


struct avl_node *
avl_remove(struct avl_root *root, struct avl_node *node);


/*
 * traversal operations
 */

struct avl_node *
avl_first(struct avl_root *root);

struct avl_node *
avl_last(struct avl_root *root);

struct avl_node *
avl_next(struct avl_root *root, struct avl_node *node);

struct avl_node *
avl_prev(struct avl_root *root, struct avl_node *node);


/*
 * internal operations (testing only)
 */

int
avl_validate(struct avl_root *root);

usize
avl_size(struct avl_root *root);
