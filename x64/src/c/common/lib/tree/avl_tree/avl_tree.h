#ifndef _AVL_TREE_H_
#define _AVL_TREE_H_
#include "../../../sys/kdef.h"

typedef struct _avl_tree_node_t
{
        struct _avl_tree_node_t * left;
        struct _avl_tree_node_t * right;
        struct _avl_tree_node_t * parent;
        int height;
} avl_tree_node_t;

typedef struct
{
        avl_tree_node_t * root;
        int size;
} avl_tree_t;


avl_tree_node_t * SAPI avl_tree_search(avl_tree_t *tree, avl_tree_node_t * node, int (*compare)(void *, void *));

void SAPI avl_tree_insert(avl_tree_t *tree, void *data, int (*compare)(void *, void *));

void SAPI avl_tree_delete(avl_tree_t *tree, void *data, int (*compare)(void *, void *));

void SAPI avl_tree_init(avl_tree_t * tree);

#endif