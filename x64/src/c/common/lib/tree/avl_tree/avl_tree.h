#ifndef _AVL_TREE_H_
#define _AVL_TREE_H_
#include "../../../sys/kdef.h"
#include "../../../sys/sys_info.h"

typedef struct _avl_tree_entry_t
{
        struct _avl_tree_entry_t * left;
        struct _avl_tree_entry_t * right;
        struct _avl_tree_entry_t * parent;
        linked_list_t element_list;
        linked_list_entry_t list_entry;
        int height;
} avl_tree_entry_t;

typedef struct
{
        avl_tree_entry_t * root;
} avl_tree_t;


avl_tree_entry_t * SAPI avl_tree_search(avl_tree_t *tree, avl_tree_entry_t * node, int (*compare)(avl_tree_entry_t *, avl_tree_entry_t *));

void SAPI avl_tree_insert(avl_tree_t *tree, void *data, int (*compare)(avl_tree_entry_t *, avl_tree_entry_t *));

void SAPI avl_tree_delete(avl_tree_t *tree, void *data, int (*compare)(avl_tree_entry_t *, avl_tree_entry_t *));

void SAPI avl_tree_init(avl_tree_t * tree);

avl_tree_entry_t* SAPI avl_tree_largest(avl_tree_t *tree);

avl_tree_entry_t* SAPI avl_tree_smallest(avl_tree_t *tree);

avl_tree_entry_t* SAPI avl_tree_larger(avl_tree_entry_t* entry);

avl_tree_entry_t* SAPI avl_tree_smaller(avl_tree_entry_t* entry);

#endif