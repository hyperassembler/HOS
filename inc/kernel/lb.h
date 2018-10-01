#pragma once

#include "common.h"
#include "kernel/status.h"

/*
 //Not used for now
 //BST interface

struct bstree;

struct bstree_node
{
	struct bstree_node *left;
	struct bstree_node *treenode;
};

struct bstree_impl
{
	struct bstree_node *(KAPI *t_search)(struct bstree *tree, struct bstree_node *entry);

	k_status (KAPI *t_insert)(struct bstree *tree, struct bstree_node *entry);

	k_status (KAPI *t_delete)(struct bstree *tree, struct bstree_node *entry, struct bstree_node **out);

	int32 (KAPI *t_size)(struct bstree *tree);

	struct bstree_node *(KAPI *t_max)(struct bstree *tree);

	struct bstree_node *(KAPI *t_min)(struct bstree *tree);

	struct bstree_node *(KAPI *t_prev)(struct bstree_node *tree);

	struct bstree_node *(KAPI *t_next)(struct bstree_node *tree);

	k_status (KAPI *t_validate)(struct bstree *tree);
};

typedef int32 (KAPI *bstree_cmp_fp)(struct bstree_node *left, struct bstree_node *treenode);

struct bstree
{
	struct bstree_impl *impl;
	struct tree_node *root;
	bstree_cmp_fp cmp;
};

*/

/**
 * AVL tree
 */
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
typedef int32 (*atree_cmp_fp)(
        struct atree_node *tree_node,
        struct atree_node *self);

struct atree
{
    atree_cmp_fp cmpf;
    struct atree_node *root;
};


struct atree_node *
lb_atree_search(
        struct atree *tree,
        struct atree_node *entry);


struct atree_node *
lb_atree_insert(
        struct atree *tree,
        struct atree_node *entry);


struct atree_node *
lb_atree_delete(
        struct atree *tree,
        struct atree_node *entry);


void
lb_atree_init(
        struct atree *tree,
        atree_cmp_fp compare);


struct atree_node *
lb_atree_max(
        struct atree *tree);


struct atree_node *
lb_atree_min(
        struct atree *tree);


struct atree_node *
lb_atree_next(
        struct atree *tree,
        struct atree_node *entry);


struct atree_node *
lb_atree_prev(
        struct atree *tree,
        struct atree_node *entry);

bool
lb_atree_validate(
        struct atree *tree);

uint32
lb_atree_size(
        struct atree *tree);


/**
 * Linked list
 */

struct llist_node
{
    struct llist_node *prev;
    struct llist_node *next;
};

struct llist
{
    struct llist_node *head;
    struct llist_node *tail;
    uint32 size;
};

void
lb_llist_init(struct llist *list);

uint32
lb_llist_size(struct llist *list);

void
lb_llist_push_front(struct llist *list, struct llist_node *node);

void
lb_llist_push_back(struct llist *list, struct llist_node *node);

struct llist_node *
lb_llist_pop_front(struct llist *list);


struct llist_node *
lb_llist_pop_back(struct llist *list);

void
lb_llist_insert_by_idx(struct llist *list, uint32 index, struct llist_node *node);

struct llist_node *
lb_llist_remove_by_idx(struct llist *list, uint32 index);


struct llist_node *
lb_llist_get(struct llist *list, uint32 index);


void
lb_llist_insert_by_ref(struct llist *list, struct llist_node *cur_node, struct llist_node *new_node);


struct llist_node *
lb_llist_remove_by_ref(struct llist *list, struct llist_node *node);


struct llist_node *
lb_llist_next(struct llist_node *node);


struct llist_node *
lb_llist_prev(struct llist_node *node);


struct llist_node *
lb_llist_first(struct llist *list);


struct llist_node *
lb_llist_last(struct llist *list);


/**
 * SALLOC
 */

void
lb_salloc_init(void *base, uint32 size);

void *
lb_salloc(void *base, uint32 size);

void
lb_sfree(void *base, void *ptr);

bool
lb_salloc_assert(void *base, const uint32 *blk_size, const bool *blk_free, uint32 size);

