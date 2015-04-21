#ifndef _AVL_TREE_H_
#define _AVL_TREE_H_
#include "../../../kdef.h"

typedef struct __attribute__((packed)) _avl_tree_node
{
        struct _avl_tree_node * left;
        struct _avl_tree_node * right;
        struct _avl_tree_node * parent;
        int height;
} avl_tree_node;

typedef struct __attribute__((packed))
{
        avl_tree_node * root;
        int size;
} avl_tree;

avl_tree_node* NATIVE64 avl_tree_node_insert(avl_tree_node* root, avl_tree_node* node, int(*compare)(void*,void*));

avl_tree_node* NATIVE64 avl_tree_node_delete(avl_tree_node* root, avl_tree_node* node, int (*compare)(void *, void *));

avl_tree_node* NATIVE64 avl_tree_node_search(avl_tree_node *root, avl_tree_node* node, int(*compare)(void *, void *));

void NATIVE64 avl_tree_node_init(avl_tree_node* it);

avl_tree_node* NATIVE64 avl_tree_node_next(avl_tree_node *it);

avl_tree_node* NATIVE64 avl_tree_node_prev(avl_tree_node *it);

avl_tree_node * NATIVE64 avl_tree_node_smallest(avl_tree_node *root);

avl_tree_node * NATIVE64 avl_tree_node_largest(avl_tree_node *root);



avl_tree_node* avl_tree_search(avl_tree *tree, avl_tree_node* node, int (*compare)(void *, void *));

void NATIVE64 avl_tree_insert(avl_tree *tree, void *data, int (*compare)(void *, void *));

void NATIVE64 avl_tree_delete(avl_tree *tree, void *data, int (*compare)(void *, void *));

void NATIVE64 avl_tree_init(avl_tree* tree);

// TESTS

int NATIVE64 avl_tree_node_calculate_height(avl_tree_node *tree);

int NATIVE64 avl_tree_node_test(avl_tree_node *tree, int(*compare)(void *, void *));

#endif