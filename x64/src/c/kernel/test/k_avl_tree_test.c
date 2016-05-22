//#include "avl_tree.h"
//#include "std_lib.h"
//#include "k_lib_test.h"
//
//typedef struct __attribute__ ((packed))
//{
//    avl_tree_entry_t node;
//    int val;
//} int_node;
//
//
//// TESTS
//
//static int SAPI avl_tree_node_calculate_height(avl_tree_entry_t *tree)
//{
//    if (tree == NULL)
//        return -1;
//    return max_32(avl_tree_node_calculate_height(tree->left), avl_tree_node_calculate_height(tree->right)) + 1;
//}
//
//static int SAPI _avl_tree_node_get_balance_factor(avl_tree_entry_t* entry)
//{
//    return entry == NULL ? -1 : entry->height;
//}
//
//static int SAPI avl_tree_node_test(avl_tree_entry_t *tree, int (*compare)(void*, void*))
//{
//    if (tree == NULL)
//        return 1;
//    if (_avl_tree_node_get_balance_factor(tree) < -1 || _avl_tree_node_get_balance_factor(tree) > 1 || avl_tree_node_calculate_height(tree) != tree->height)
//        return 0;
//    if(tree->left != NULL)
//    {
//        if(tree->left->parent != tree)
//            return 0;
//    }
//    if(tree->right != NULL)
//    {
//        if(tree->right->parent != tree)
//            return 0;
//    }
//    if(compare != NULL)
//    {
//        if((tree->right != NULL && compare(tree,tree->right) > 0) || (tree->left != NULL && compare(tree,tree->left) < 0))
//            return 0;
//    }
//    return avl_tree_node_test(tree->left,compare) && avl_tree_node_test(tree->right,compare);
//}
//
//int compare(void* a, void* b)
//{
//    int_node* aa = (int_node*)a;
//    int_node* bb = (int_node*)b;
//    if(aa->val > bb->val)
//        return 1;
//    else if(aa->val < bb->val)
//        return -1;
//    return 0;
//}
//
//void in_order_print(avl_tree_entry_t *tree)
//{
//    if (tree == NULL)
//        return;
//    avl_tree_entry_t* node = avl_tree_entry_t(tree);
//    while(node != NULL)
//    {
//        printf("%d ", ((int_node*)node)->val);
//        node = avl_tree_node_prev(node);
//    }
//    return;
//}
//
//int_node* create_int_node(int val)
//{
//    int_node* node = (int_node*)malloc(sizeof(int_node));
//    avl_tree_node_init(&node->node);
//    node->val = val;
//    return node;
//}
//
//int avl_tree_test (void)
//{
//    int_node* val[1000];
//    srand((unsigned int)time(NULL));
//    avl_tree* avlTree = (avl_tree*)malloc(sizeof(avl_tree));
//    avl_tree_init(avlTree);
//    //test INSERT general
//    int i = 0;
//    for(i = 0; i < 1000; i++)
//    {
//        val[i] = create_int_node(rand()%10000);
//        avl_tree_insert(avlTree,(avl_tree_node*)val[i],compare);
//        assert(avl_tree_node_test(avlTree->root,compare));
//    }
//    //test Delete general
//    for(i = 0; i < 1000; i++)
//    {
//        avl_tree_delete(avlTree,val[i],compare);
//        assert(avl_tree_node_test(avlTree->root,compare));
//        free(val[i]);
//    }
//
//    //test delete visualized
//    for(i = 0; i < 20; i++)
//    {
//        val[i] = create_int_node(rand()%2000);
//        avl_tree_insert(avlTree,(avl_tree_node*)val[i],compare);
//        assert(avl_tree_node_test(avlTree->root,compare));
//    }
//    in_order_print(avlTree->root);
//    for(i = 0; i < 20; i++)
//    {
//        avl_tree_delete(avlTree,(avl_tree_node*)val[i],compare);
//        printf("\nDeleting: %d\n",val[i]->val);
//        in_order_print(avlTree->root);
//        assert(avl_tree_node_test(avlTree->root,compare));
//        free(val[i]);
//    }
//    free(avlTree);
//    return 0;
//}