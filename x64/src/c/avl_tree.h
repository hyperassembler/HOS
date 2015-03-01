#ifndef _AVL_TREE_H_
#define _AVL_TREE_H_

typedef struct __attribute__((packed)) _avl_tree
{
        struct _avl_tree * left;
        struct _avl_tree * right;
        struct _avl_tree * parent;
        int height;
        void *data;
} avl_tree;
int avl_test(avl_tree* avl_tree);
int avl_test_calculate_height(avl_tree *avl_tree);
avl_tree* avl_smallest(avl_tree* root);
avl_tree* avl_largest(avl_tree* root);
avl_tree* avl_next(avl_tree* root);
avl_tree* avl_prev(avl_tree* root);
int avl_size(avl_tree* root);
void* avl_read(avl_tree *root);
avl_tree* avl_search(avl_tree* root, void* data, int(*compare)(void*,void*));
avl_tree* avl_create();
avl_tree* avl_insert(avl_tree* root, void* data, int(*compare)(void*,void*));
avl_tree* avl_delete(avl_tree* root, void* data, int(*compare)(void*,void*));
void avl_free(avl_tree *root, void (*delete_data)(void*));
#endif