#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include "avl_tree.h"
typedef struct __attribute__ ((packed))
{
    avl_tree_node node;
    int val;
} int_node;

int compare(void* a, void* b)
{
    int_node* aa = (int_node*)a;
    int_node* bb = (int_node*)b;
    if(aa->val > bb->val)
        return 1;
    else if(aa->val < bb->val)
        return -1;
    return 0;
}

void in_order_print(avl_tree_node *tree)
{
    if (tree == NULL)
        return;
    avl_tree_node* node = avl_tree_node_largest(tree);
    while(node != NULL)
    {
        printf("%d ", ((int_node*)node)->val);
        node = avl_tree_node_prev(node);
    }
    return;
}

int_node* create_int_node(int val)
{
    int_node* node = (int_node*)malloc(sizeof(int_node));
    avl_tree_node_init(&node->node);
    node->val = val;
    return node;
}

int main (void)
{
    int_node* val[1000];
    srand((unsigned int)time(NULL));
    avl_tree* avlTree = (avl_tree*)malloc(sizeof(avl_tree));
    avl_tree_init(avlTree);
    //test INSERT general
    int i = 0;
    for(i = 0; i < 1000; i++)
    {
        val[i] = create_int_node(rand()%10000);
        avl_tree_insert(avlTree,(avl_tree_node*)val[i],compare);
        assert(avl_tree_node_test(avlTree->root,compare));
    }
    //test Delete general
    for(i = 0; i < 1000; i++)
    {
        avl_tree_delete(avlTree,val[i],compare);
        assert(avl_tree_node_test(avlTree->root,compare));
        free(val[i]);
    }

    //test delete visualized
    for(i = 0; i < 20; i++)
    {
        val[i] = create_int_node(rand()%2000);
        avl_tree_insert(avlTree,(avl_tree_node*)val[i],compare);
        assert(avl_tree_node_test(avlTree->root,compare));
    }
    in_order_print(avlTree->root);
    for(i = 0; i < 20; i++)
    {
        avl_tree_delete(avlTree,(avl_tree_node*)val[i],compare);
        printf("\nDeleting: %d\n",val[i]->val);
        in_order_print(avlTree->root);
        assert(avl_tree_node_test(avlTree->root,compare));
        free(val[i]);
    }
    free(avlTree);
    return 0;
}