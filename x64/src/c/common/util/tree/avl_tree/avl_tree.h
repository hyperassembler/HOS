#ifndef _AVL_TREE_H_
#define _AVL_TREE_H_
#include "../../../kdef.h"

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

avl_tree_node_t * NATIVE64 avl_tree_node_insert(avl_tree_node_t * root, avl_tree_node_t * node, int(*compare)(void*,void*));

avl_tree_node_t * NATIVE64 avl_tree_node_delete(avl_tree_node_t * root, avl_tree_node_t * node, int (*compare)(void *, void *));

avl_tree_node_t * NATIVE64 avl_tree_node_search(avl_tree_node_t *root, avl_tree_node_t * node, int(*compare)(void *, void *));

void NATIVE64 avl_tree_node_init(avl_tree_node_t * it);

avl_tree_node_t * NATIVE64 avl_tree_node_next(avl_tree_node_t *it);

avl_tree_node_t * NATIVE64 avl_tree_node_prev(avl_tree_node_t *it);

avl_tree_node_t * NATIVE64 avl_tree_node_smallest(avl_tree_node_t *root);

avl_tree_node_t * NATIVE64 avl_tree_node_largest(avl_tree_node_t *root);



avl_tree_node_t * avl_tree_search(avl_tree_t *tree, avl_tree_node_t * node, int (*compare)(void *, void *));

void NATIVE64 avl_tree_insert(avl_tree_t *tree, void *data, int (*compare)(void *, void *));

void NATIVE64 avl_tree_delete(avl_tree_t *tree, void *data, int (*compare)(void *, void *));

void NATIVE64 avl_tree_init(avl_tree_t * tree);

// TESTS

int NATIVE64 avl_tree_node_calculate_height(avl_tree_node_t *tree);

int NATIVE64 avl_tree_node_test(avl_tree_node_t *tree, int(*compare)(void *, void *));

#endif