#ifndef _AVL_TREE_H_
#define _AVL_TREE_H_
#include "kdef.h"

typedef struct __attribute__((packed)) _avl_tree_node
{
        struct _avl_tree_node * left;
        struct _avl_tree_node * right;
        struct _avl_tree_node * parent;
        int height;
        void *data;
} avl_tree_node;

typedef struct __attribute__((packed))
{
        avl_tree_node * root;
        int size;
} avl_tree;

typedef struct __attribute__((packed))
{
        avl_tree_node * current;
} avl_tree_iterator;

void NATIVE64 avl_tree_insert(avl_tree *tree, void *data, int (*compare)(void *, void *));

void NATIVE64 avl_tree_delete(avl_tree *tree, void *data, int (*compare)(void *, void *));

void NATIVE64 avl_tree_free(avl_tree *tree, void (*delete_data)(void *));

void NATIVE64 *avl_tree_search(avl_tree *tree, void *data, int(*compare)(void *, void *));

avl_tree * NATIVE64 avl_tree_create();

avl_tree_iterator* NATIVE64 avl_tree_create_iterator(avl_tree *tree);

void NATIVE64 avl_tree_next(avl_tree_iterator *it);

void NATIVE64 avl_tree_prev(avl_tree_iterator *it);

int NATIVE64 avl_tree_test_calculate_height(avl_tree_node *tree);

int NATIVE64 avl_tree_test(avl_tree_node *tree);

#endif