#ifndef _AVL_TREE_H_
#define _AVL_TREE_H_

#include "std_lib.h"

typedef struct _avl_tree_entry_t
{
    struct _avl_tree_entry_t *left;
    struct _avl_tree_entry_t *right;
    struct _avl_tree_entry_t *parent;
    int height;
} avl_tree_entry_t;

typedef struct
{
    avl_tree_entry_t *root;
} avl_tree_t;


avl_tree_entry_t *SAPI avl_tree_search(avl_tree_t *tree, avl_tree_entry_t *entry,
                                       int32_t (*compare)(avl_tree_entry_t *, avl_tree_entry_t *));

void SAPI avl_tree_insert(avl_tree_t *tree, avl_tree_entry_t *entry,
                          int32_t (*compare)(avl_tree_entry_t *, avl_tree_entry_t *));

void SAPI avl_tree_delete(avl_tree_t *tree, avl_tree_entry_t *entry,
                          int32_t (*compare)(avl_tree_entry_t *, avl_tree_entry_t *));

void SAPI avl_tree_init(avl_tree_t *tree);

avl_tree_entry_t *SAPI avl_tree_largest(avl_tree_t *tree);

avl_tree_entry_t *SAPI avl_tree_smallest(avl_tree_t *tree);

avl_tree_entry_t *SAPI avl_tree_larger(avl_tree_entry_t *entry);

avl_tree_entry_t *SAPI avl_tree_smaller(avl_tree_entry_t *entry);

bool SAPI avl_tree_validate(avl_tree_t *tree, int32_t (*compare)(avl_tree_entry_t *, avl_tree_entry_t *));

#endif
