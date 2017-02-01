#include "k_test_driver.h"
#include "../lib/inc/avl_tree.h"

typedef struct
{
    avl_tree_node_t tree_entry;
    int val;
} int_tree_node;

static int_tree_node *create_tree_node(int val)
{
    int_tree_node *rs = talloc(sizeof(int_tree_node));
    rs->val = val;
    return rs;
}

static int32_t compare(void *root1, void *node1)
{
    avl_tree_node_t *root = (avl_tree_node_t*)node1;
    avl_tree_node_t *node = (avl_tree_node_t*)root1;
    int_tree_node *rooti = OBTAIN_STRUCT_ADDR(root, int_tree_node, tree_entry);
    int_tree_node *nodei = OBTAIN_STRUCT_ADDR(node, int_tree_node, tree_entry);
    return rooti->val - nodei->val;
}

//static void _pre_order(avl_tree_node_t *node, bool root)
//{
//    if (node == NULL)
//        return;
//    int_tree_node *my_node = OBTAIN_STRUCT_ADDR(node, tree_entry, int_tree_node);
//    printf("%d-", my_node->val);
//    _pre_order(node->left, false);
//    _pre_order(node->right, false);
//    if (root)
//        printf("\n");
//}
//
//static void pre_order(avl_tree_node_t *node)
//{
//    _pre_order(node, true);
//}

static int counter = 0;

static bool _pre_order_assert(avl_tree_node_t *node, int order[], int size)
{
    if (node == NULL)
        return true;
    if (counter >= size)
        return false;

    bool result = true;
    int_tree_node *my_node = OBTAIN_STRUCT_ADDR(node, int_tree_node, tree_entry);
    if (order[counter] != my_node->val)
    {
        result = false;
    }
    counter++;
    result = result && _pre_order_assert(node->left, order, size);
    result = result && _pre_order_assert(node->right, order, size);
    return result;
}

static bool pre_order_assert(avl_tree_t *node, int order[], int size)
{
    counter = 0;
    return _pre_order_assert(node->root, order, size);
}

//////// TESTS/////////

static bool insert_simple_l()
{
    //1                   2
    // \                 / \
    //  2   == 1L ==>   1   3
    //   \
    //    3

    bool result = true;
    avl_tree_t tree;
    lb_avl_tree_init(&tree, compare);

    lb_avl_tree_insert(&tree, &create_tree_node(1)->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(2)->tree_entry);
    int val1[] = {1, 2};
    result = result && pre_order_assert(&tree, val1, 2);

    lb_avl_tree_insert(&tree, &create_tree_node(3)->tree_entry);

    int val2[] = {2, 1, 3};
    result = result && pre_order_assert(&tree, val2, 3);
    return result && lb_avl_tree_validate(&tree);
}

static bool insert_simple_r()
{
    //    3               2
    //   /               / \
    //  2   == 1R ==>   1   3
    // /
    //1

    bool result = true;
    avl_tree_t tree;
    lb_avl_tree_init(&tree, compare);

    lb_avl_tree_insert(&tree, &create_tree_node(3)->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(2)->tree_entry);
    int val1[] = {3, 2};
    result = result && pre_order_assert(&tree, val1, 2);

    lb_avl_tree_insert(&tree, &create_tree_node(1)->tree_entry);

    int val2[] = {2, 1, 3};
    result = result && pre_order_assert(&tree, val2, 3);
    return result && lb_avl_tree_validate(&tree);
}

static bool insert_simple_ll()
{
    //2                  3
    // \                / \
    //  4   == 2L ==>  2   4
    // /
    //3
    bool result = true;
    avl_tree_t tree;
    lb_avl_tree_init(&tree, compare);

    lb_avl_tree_insert(&tree, &create_tree_node(2)->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(4)->tree_entry);
    int val1[] = {2, 4};
    result = result && pre_order_assert(&tree, val1, 2);

    lb_avl_tree_insert(&tree, &create_tree_node(3)->tree_entry);

    int val2[] = {3, 2, 4};
    result = result && pre_order_assert(&tree, val2, 3);
    return result && lb_avl_tree_validate(&tree);
}

static bool insert_simple_rr()
{
    //  4                3
    // /                / \
    //2     == 2R ==>  2   4
    // \
    //  3
    bool result = true;
    avl_tree_t tree;
    lb_avl_tree_init(&tree, compare);

    lb_avl_tree_insert(&tree, &create_tree_node(4)->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(2)->tree_entry);
    int val1[] = {4, 2};
    result = result && pre_order_assert(&tree, val1, 2);

    lb_avl_tree_insert(&tree, &create_tree_node(3)->tree_entry);

    int val2[] = {3, 2, 4};
    result = result && pre_order_assert(&tree, val2, 3);
    return result && lb_avl_tree_validate(&tree);
}

static bool insert_complex_1()
{
    //    20+          20++           20++         9
    //   /  \         /  \           /  \         / \
    //  4    26 =>   4-   26 =>     9+   26 =>   4+  20
    // / \          / \            / \          /   /  \
    //3   9        3   9-         4+  15       3  15    26
    //                   \       /
    //                    15    3
    bool result = true;
    avl_tree_t tree;
    lb_avl_tree_init(&tree, compare);

    lb_avl_tree_insert(&tree, &create_tree_node(20)->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(4)->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(26)->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(3)->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(9)->tree_entry);
    int val1[] = {20, 4, 3, 9, 26};
    result = result && pre_order_assert(&tree, val1, 5);

    lb_avl_tree_insert(&tree, &create_tree_node(15)->tree_entry);

    int val2[] = {9, 4, 3, 20, 15, 26};
    result = result && pre_order_assert(&tree, val2, 6);
    return result && lb_avl_tree_validate(&tree);
}

static bool insert_complex_2()
{
    //    20+          20++           20++         9
    //   /  \         /  \           /  \         / \
    //  4    26 =>   4-   26 =>     9++  26 =>   4   20-
    // / \          / \            /            / \    \
    //3   9        3   9+         4            3   8    26
    //                /          / \
    //               8          3   8
    bool result = true;
    avl_tree_t tree;
    lb_avl_tree_init(&tree, compare);

    lb_avl_tree_insert(&tree, &create_tree_node(20)->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(4)->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(26)->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(3)->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(9)->tree_entry);
    int val1[] = {20, 4, 3, 9, 26};
    result = result && pre_order_assert(&tree, val1, 5);

    lb_avl_tree_insert(&tree, &create_tree_node(8)->tree_entry);

    int val2[] = {9, 4, 3, 8, 20, 26};
    result = result && pre_order_assert(&tree, val2, 6);
    return result && lb_avl_tree_validate(&tree);
}

static bool insert_complex_3()
{
    //      __20+__                _20++_                  __20++_                ___9___
    //     /       \              /      \                /       \              /       \
    //    4         26    =>     4-       26    =>       9+        26    =>     4+      __20__
    //   / \       /  \         / \      /  \           / \       /  \         / \     /      \
    //  3+  9    21    30      3+  9-  21    30        4+  11-  21    30      3+  7  11-       26
    // /   / \                /   / \                 / \   \                /         \      /  \
    //2   7   11             2   7   11-             3+  7   15             2           15  21    30
    //                                 \            /
    //                                  15         2
    bool result = true;
    avl_tree_t tree;
    lb_avl_tree_init(&tree, compare);

    lb_avl_tree_insert(&tree, &create_tree_node(20)->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(4)->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(26)->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(3)->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(9)->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(21)->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(30)->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(2)->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(7)->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(11)->tree_entry);
    int val1[] = {20, 4, 3, 2, 9, 7, 11, 26, 21, 30};
    result = result && pre_order_assert(&tree, val1, 10);

    lb_avl_tree_insert(&tree, &create_tree_node(15)->tree_entry);

    int val2[] = {9, 4, 3, 2, 7, 20, 11, 15, 26, 21, 30};
    result = result && pre_order_assert(&tree, val2, 11);
    return result && lb_avl_tree_validate(&tree);
}

static bool insert_complex_4()
{
    //      __20+__                _20++_                  __20++_                ___9___
    //     /       \              /      \                /       \              /       \
    //    4         26           4-       26             9+        26           4        _20-
    //   / \       /  \         / \      /  \           / \       /  \         / \      /    \
    //  3+  9    21    30 =>   3+  9+  21    30 =>     4   11   21    30 =>   3+  7-  11      26
    // /   / \                /   / \                 / \                    /     \         /  \
    //2   7   11             2   7-  11              3+  7-                 2       8      21    30
    //                            \                 /     \
    //                             8               2       8
    bool result = true;
    avl_tree_t tree;
    lb_avl_tree_init(&tree, compare);

    lb_avl_tree_insert(&tree, &create_tree_node(20)->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(4)->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(26)->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(3)->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(9)->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(21)->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(30)->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(2)->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(7)->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(11)->tree_entry);
    int val1[] = {20, 4, 3, 2, 9, 7, 11, 26, 21, 30};
    result = result && pre_order_assert(&tree, val1, 10);

    lb_avl_tree_insert(&tree, &create_tree_node(8)->tree_entry);

    int val2[] = {9, 4, 3, 2, 7, 8, 20, 11, 26, 21, 30};
    result = result && pre_order_assert(&tree, val2, 11);
    return result && lb_avl_tree_validate(&tree);
}

static bool insert_duplicate()
{
    //      __20+__                _20++_                  __20++_                ___9___
    //     /       \              /      \                /       \              /       \
    //    4         26           4-       26             9+        26           4        _20-
    //   / \       /  \         / \      /  \           / \       /  \         / \      /    \
    //  3+  9    21    30 =>   3+  9+  21    30 =>     4   11   21    30 =>   3+  7-  11      26
    // /   / \                /   / \                 / \                    /     \         /  \
    //2   7   11             2   7-  11              3+  7-                 2       8      21    30
    //                            \                 /     \
    //                             8               2       8
    bool result = true;
    avl_tree_t tree;
    lb_avl_tree_init(&tree, compare);

    lb_avl_tree_insert(&tree, &create_tree_node(20)->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(4)->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(26)->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(3)->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(9)->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(21)->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(30)->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(2)->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(7)->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(11)->tree_entry);
    int val1[] = {20, 4, 3, 2, 9, 7, 11, 26, 21, 30};
    result = result && pre_order_assert(&tree, val1, 10);

    lb_avl_tree_insert(&tree, &create_tree_node(20)->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(30)->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(7)->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(2)->tree_entry);

    result = result && pre_order_assert(&tree, val1, 10);
    return result && lb_avl_tree_validate(&tree);
}


static bool delete_simple_l()
{
    //  2                   3
    // x \                 / \
    //1   3   == 1L ==>   2   4
    //     \
    //      4

    bool result = true;
    avl_tree_t tree;
    lb_avl_tree_init(&tree, compare);

    int_tree_node *deleted = create_tree_node(1);

    lb_avl_tree_insert(&tree, &create_tree_node(2)->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(3)->tree_entry);
    lb_avl_tree_insert(&tree, &deleted->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(4)->tree_entry);
    int val1[] = {2, 1, 3, 4};
    result = result && pre_order_assert(&tree, val1, 4);

    lb_avl_tree_delete(&tree, &deleted->tree_entry);

    int val2[] = {3, 2, 4};
    result = result && pre_order_assert(&tree, val2, 3);
    return result && lb_avl_tree_validate(&tree);
}

static bool delete_simple_r()
{
    //    3                  2
    //   / x                / \
    //  2   4  == 1R ==>   1   3
    // /
    //1

    bool result = true;
    avl_tree_t tree;
    lb_avl_tree_init(&tree, compare);

    int_tree_node *deleted = create_tree_node(4);

    lb_avl_tree_insert(&tree, &create_tree_node(3)->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(2)->tree_entry);
    lb_avl_tree_insert(&tree, &deleted->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(1)->tree_entry);
    int val1[] = {3, 2, 1, 4};
    result = result && pre_order_assert(&tree, val1, 4);

    lb_avl_tree_delete(&tree, &deleted->tree_entry);

    int val2[] = {2, 1, 3};
    result = result && pre_order_assert(&tree, val2, 3);
    return result && lb_avl_tree_validate(&tree);
}

static bool delete_simple_ll()
{
    //  2                  3
    // x \                / \
    //1   4   == 2L ==>  2   4
    //   /
    //  3
    bool result = true;
    avl_tree_t tree;
    lb_avl_tree_init(&tree, compare);

    int_tree_node *deleted = create_tree_node(1);

    lb_avl_tree_insert(&tree, &create_tree_node(2)->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(4)->tree_entry);
    lb_avl_tree_insert(&tree, &deleted->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(3)->tree_entry);
    int val1[] = {2, 1, 4, 3};
    result = result && pre_order_assert(&tree, val1, 4);

    lb_avl_tree_delete(&tree, &deleted->tree_entry);

    int val2[] = {3, 2, 4};
    result = result && pre_order_assert(&tree, val2, 3);
    return result && lb_avl_tree_validate(&tree);
}

static bool delete_simple_rr()
{
    //  3                  2
    // / x                / \
    //2   4   == 2R ==>  1   3
    // \
    //  1
    bool result = true;
    avl_tree_t tree;
    lb_avl_tree_init(&tree, compare);

    int_tree_node *deleted = create_tree_node(4);

    lb_avl_tree_insert(&tree, &create_tree_node(3)->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(2)->tree_entry);
    lb_avl_tree_insert(&tree, &deleted->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(1)->tree_entry);
    int val1[] = {3, 2, 1, 4};
    result = result && pre_order_assert(&tree, val1, 4);

    lb_avl_tree_delete(&tree, &deleted->tree_entry);

    int val2[] = {2, 1, 3};
    result = result && pre_order_assert(&tree, val2, 3);
    return result && lb_avl_tree_validate(&tree);
}

static bool delete_complex_1()
{
    // Test Case #1
    //  - A single node tree has its only node removed.
    // Create:
    //                     10
    //
    // Call: remove(10)
    //
    // Result:
    //                    empty tree
    bool result = true;
    avl_tree_t tree;
    lb_avl_tree_init(&tree, compare);

    int_tree_node *deleted = create_tree_node(10);

    lb_avl_tree_insert(&tree, &deleted->tree_entry);
    int val1[] = {10};
    result = result && pre_order_assert(&tree, val1, 1);

    lb_avl_tree_delete(&tree, &deleted->tree_entry);

    result = result && pre_order_assert(&tree, val1, 0);
    return result && lb_avl_tree_validate(&tree);
}

static bool delete_complex_2()
{
    // Test Case #2
    //  - A small tree has its root removed.
    // Create:
    //                     20
    //                    /  \
	//                  10    30
    //                       /  \
	//                      25  35
    //
    // Call: remove(20)
    //
    // Results: (simplest result with no rotations)
    //          (replace root with smallest value on the right or 25)
    //
    //                     25
    //                    /  \
	//                  10    30
    //                         \
	//                         35
    //
    bool result = true;
    avl_tree_t tree;
    lb_avl_tree_init(&tree, compare);

    int_tree_node *deleted = create_tree_node(20);

    lb_avl_tree_insert(&tree, &deleted->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(10)->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(30)->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(25)->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(35)->tree_entry);
    int val1[] = {20, 10, 30, 25, 35};
    result = result && pre_order_assert(&tree, val1, 5);

    lb_avl_tree_delete(&tree, &deleted->tree_entry);

    int val2[] = {25, 10, 30, 35};
    result = result && pre_order_assert(&tree, val2, 4);
    return result && lb_avl_tree_validate(&tree);
}

static bool delete_complex_3()
{
    // Test Case #3
    //  - A small tree has a node with 2 children removed
    //                     20
    //                    /  \
	//                  10    30
    //                 /  \  /
    //                5  15 25
    //
    // Call: remove(10)
    //
    // Results:
    //                     20
    //                    /  \
	//                  15    30
    //                 /     /
    //                5     25
    bool result = true;
    avl_tree_t tree;
    lb_avl_tree_init(&tree, compare);

    int_tree_node *deleted = create_tree_node(10);

    lb_avl_tree_insert(&tree, &create_tree_node(20)->tree_entry);
    lb_avl_tree_insert(&tree, &deleted->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(30)->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(5)->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(15)->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(25)->tree_entry);
    int val1[] = {20, 10, 5, 15, 30, 25};
    result = result && pre_order_assert(&tree, val1, 6);

    lb_avl_tree_delete(&tree, &deleted->tree_entry);

    int val2[] = {20, 15, 5, 30, 25};
    result = result && pre_order_assert(&tree, val2, 5);
    return result && lb_avl_tree_validate(&tree);
}

static bool delete_complex_4()
{
    // Test Case #4
    //  - A small tree has all nodes but the root removed from the bottom up.
    // Create:
    //                     20
    //                    /  \
	//                  10    30
    //                 /  \  /
    //                5  15 25
    //
    // Call: remove(5), remove(15), remove(25), remove(10), remove(30)
    //
    //
    // Results:
    //                     20
    //
    bool result = true;
    avl_tree_t tree;
    lb_avl_tree_init(&tree, compare);

    int_tree_node *delete5 = create_tree_node(5);
    int_tree_node *delete10 = create_tree_node(10);
    int_tree_node *delete15 = create_tree_node(15);
    int_tree_node *delete25 = create_tree_node(25);
    int_tree_node *delete30 = create_tree_node(30);


    lb_avl_tree_insert(&tree, &create_tree_node(20)->tree_entry);
    lb_avl_tree_insert(&tree, &delete10->tree_entry);
    lb_avl_tree_insert(&tree, &delete30->tree_entry);
    lb_avl_tree_insert(&tree, &delete5->tree_entry);
    lb_avl_tree_insert(&tree, &delete15->tree_entry);
    lb_avl_tree_insert(&tree, &delete25->tree_entry);

    int val1[] = {20, 10, 5, 15, 30, 25};
    result = result && pre_order_assert(&tree, val1, 6);

    lb_avl_tree_delete(&tree, &delete5->tree_entry);
    lb_avl_tree_delete(&tree, &delete15->tree_entry);
    lb_avl_tree_delete(&tree, &delete25->tree_entry);
    lb_avl_tree_delete(&tree, &delete10->tree_entry);
    lb_avl_tree_delete(&tree, &delete30->tree_entry);

    int val2[] = {20};
    result = result && pre_order_assert(&tree, val2, 1);
    return result && lb_avl_tree_validate(&tree);
}

static bool delete_complex_single_rotation()
{
    // Test case single rotation
    //
    // Create:
    //
    //                     20
    //             /                 \
	//            10                 30
    //          /    \             /    \
	//         5     15           25    40
    //               /           /     /  \
	//             12           22    35   50
    //                               /
    //                              31
    //
    // Call: remove(50)
    //
    //                     20
    //             /                 \
	//            10                 30
    //          /    \             /    \
	//         5     15           25    35
    //               /           /     /  \
	//             12           22    31   40
    //
    //
    bool result = true;
    avl_tree_t tree;
    lb_avl_tree_init(&tree, compare);

    int_tree_node *deleted = create_tree_node(50);

    lb_avl_tree_insert(&tree, &create_tree_node(20)->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(10)->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(30)->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(5)->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(15)->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(25)->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(40)->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(12)->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(22)->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(35)->tree_entry);
    lb_avl_tree_insert(&tree, &deleted->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(31)->tree_entry);
    int val1[] = {20, 10, 5, 15, 12, 30, 25, 22, 40, 35, 31, 50};
    result = result && pre_order_assert(&tree, val1, 12);

    lb_avl_tree_delete(&tree, &deleted->tree_entry);

    int val2[] = {20, 10, 5, 15, 12, 30, 25, 22, 35, 31, 40};
    result = result && pre_order_assert(&tree, val2, 11);
    return result && lb_avl_tree_validate(&tree);
}

static bool delete_complex_double_rotation()
{
// Test case double rotation
    //
    // Create:
    //
    //                     20
    //             /                 \
	//            10                 30
    //          /    \             /    \
	//         5     15           25    40
    //               /           /     /  \
	//             12           22    35   50
    //                               /
    //                              31
    //
    // Call: remove(22)
    //
    //                     20
    //             /                 \
	//            10                 35
    //          /    \             /    \
	//         5     15           30    40
    //               /           /  \      \
	//             12           25  31     50
    //
    //
    bool result = true;
    avl_tree_t tree;
    lb_avl_tree_init(&tree, compare);

    int_tree_node *deleted = create_tree_node(22);

    lb_avl_tree_insert(&tree, &create_tree_node(20)->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(10)->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(30)->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(5)->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(15)->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(25)->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(40)->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(12)->tree_entry);
    lb_avl_tree_insert(&tree, &deleted->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(35)->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(50)->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(31)->tree_entry);
    int val1[] = {20, 10, 5, 15, 12, 30, 25, 22, 40, 35, 31, 50};
    result = result && pre_order_assert(&tree, val1, 12);

    lb_avl_tree_delete(&tree, &deleted->tree_entry);

    int val2[] = {20, 10, 5, 15, 12, 35, 30, 25, 31, 40, 50};
    result = result && pre_order_assert(&tree, val2, 11);
    return result && lb_avl_tree_validate(&tree);
}

static bool delete_complex_multiple_rotation()
{
    // Test case multiple rotation
    //
    // Create:
    //                     20
    //             /                 \
	//            10                 30
    //          /    \             /    \
	//         5     15           25    40
    //               /           /     /  \
	//             12           22    35   50
    //                               /
    //                              31
    //
    // Call: remove(5)
    //
    // Results:
    //                     30
    //             /                 \
	//            20                 40
    //          /    \             /    \
	//        12      25          35    50
    //       /  \    /           /
    //     10   15  22          31
    //
    //
    bool result = true;
    avl_tree_t tree;
    lb_avl_tree_init(&tree, compare);

    int_tree_node *deleted = create_tree_node(5);

    lb_avl_tree_insert(&tree, &create_tree_node(20)->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(10)->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(30)->tree_entry);
    lb_avl_tree_insert(&tree, &deleted->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(15)->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(25)->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(40)->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(12)->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(22)->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(35)->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(50)->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(31)->tree_entry);
    int val1[] = {20, 10, 5, 15, 12, 30, 25, 22, 40, 35, 31, 50};
    result = result && pre_order_assert(&tree, val1, 12);

    lb_avl_tree_delete(&tree, &deleted->tree_entry);

    int val2[] = {30, 20, 12, 10, 15, 25, 22, 40, 35, 31, 50};
    result = result && pre_order_assert(&tree, val2, 11);
    return result && lb_avl_tree_validate(&tree);
}

static bool delete_DNE()
{
    // Test case DNE
    //  Delete a node that does not exist
    //                     20
    //                    /  \
	//                  10    30
    //                 /  \  /
    //                5  15 25
    //
    // Call: remove(100), remove(24)
    //
    //
    // Results:
    //                     20
    //                    /  \
	//                  10    30
    //                 /  \  /
    //                5  15 25
    //
    bool result = true;
    avl_tree_t tree;
    lb_avl_tree_init(&tree, compare);

    int_tree_node *delete100 = create_tree_node(100);
    int_tree_node *delete24 = create_tree_node(24);

    lb_avl_tree_insert(&tree, &create_tree_node(20)->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(10)->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(30)->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(5)->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(15)->tree_entry);
    lb_avl_tree_insert(&tree, &create_tree_node(25)->tree_entry);

    int val1[] = {20, 10, 5, 15, 30, 25};
    result = result && pre_order_assert(&tree, val1, 6);

    lb_avl_tree_delete(&tree, &delete24->tree_entry);
    lb_avl_tree_delete(&tree, &delete100->tree_entry);
    result = result && pre_order_assert(&tree, val1, 6);
    return result && lb_avl_tree_validate(&tree);
}

#define AVL_APOCALYPSE_NUM 500
#define AVL_APOCALYPSE_ITER 2
static int_tree_node apocalypse[AVL_APOCALYPSE_NUM];

static bool test_apocalypse()
{
    bool result = true;
    avl_tree_t tree;
    lb_avl_tree_init(&tree, compare);

    // insert test
    for(int i = 0; i < AVL_APOCALYPSE_NUM; i++)
    {
        apocalypse[i].val = rand();
        while(lb_avl_tree_search(&tree, &apocalypse[i].tree_entry) != NULL)
        {
            apocalypse[i].val += rand() % 32765;
        }
        lb_avl_tree_insert(&tree, &apocalypse[i].tree_entry);
    }

    // integrity test
    result = result && lb_avl_tree_validate(&tree);
    result = result && lb_avl_tree_size(&tree) == AVL_APOCALYPSE_NUM;

    // smaller and bigger test
    avl_tree_node_t* entry = lb_avl_tree_smallest(&tree);
    uint32_t size = 0;
    int32_t prev = -1;
    int32_t cur = OBTAIN_STRUCT_ADDR(entry, int_tree_node, tree_entry)->val;
    while(entry != NULL)
    {
        if(cur < prev)
        {
            result = false;
            break;
        }
        size++;
        entry = lb_avl_tree_larger(entry);
        prev = cur;
        if(entry != NULL)
        {
            cur = OBTAIN_STRUCT_ADDR(entry, int_tree_node, tree_entry)->val;
        }
    }

    result = result && size == AVL_APOCALYPSE_NUM;

    // larger test
    entry = lb_avl_tree_largest(&tree);
    size = 0;
    cur = OBTAIN_STRUCT_ADDR(entry, int_tree_node, tree_entry)->val;
    prev = cur;
    while(entry != NULL)
    {
        if(cur > prev)
        {
            result = false;
            break;
        }
        size++;
        entry = lb_avl_tree_smaller(entry);
        prev = cur;
        if(entry != NULL)
        {
            cur = OBTAIN_STRUCT_ADDR(entry, int_tree_node, tree_entry)->val;
        }
    }

    result = result && size == AVL_APOCALYPSE_NUM;


    // delete and search test
    for(int i = 0; i < AVL_APOCALYPSE_NUM; i++)
    {
        result = result && (lb_avl_tree_search(&tree, &apocalypse[i].tree_entry) != NULL);
        lb_avl_tree_delete(&tree, &apocalypse[i].tree_entry);
        result = result && (lb_avl_tree_search(&tree, &apocalypse[i].tree_entry) == NULL);
        result = result && lb_avl_tree_validate(&tree);
    }

    result = result && (lb_avl_tree_size(&tree) == 0);
    return result;
}


void  avl_tree_test(void)
{
    test_begin("AVL tree test");

    // simple tests
    run_case("insert_simple_l", insert_simple_l());
    run_case("insert_simple_r", insert_simple_r());
    run_case("insert_simple_ll", insert_simple_ll());
    run_case("insert_simple_rr", insert_simple_rr());

    // complex ones
    run_case("insert_complex_1", insert_complex_1());
    run_case("insert_complex_2", insert_complex_2());
    run_case("insert_complex_3", insert_complex_3());
    run_case("insert_complex_4", insert_complex_4());

    // insert duplicate
    run_case("insert_duplicate", insert_duplicate());

    // simple tests
    run_case("delete_simple_l", delete_simple_l());
    run_case("delete_simple_r", delete_simple_r());
    run_case("delete_simple_ll", delete_simple_ll());
    run_case("delete_simple_rr", delete_simple_rr());

    // complex tests
    run_case("delete_complex_1", delete_complex_1());
    run_case("delete_complex_2", delete_complex_2());
    run_case("delete_complex_3", delete_complex_3());
    run_case("delete_complex_4", delete_complex_4());
    run_case("delete_complex_single_rotation", delete_complex_single_rotation());
    run_case("delete_complex_double_rotation", delete_complex_double_rotation());
    run_case("delete_complex_multiple_rotation", delete_complex_multiple_rotation());

    // delete non-existing
    run_case("delete_DNE", delete_DNE());

    srand(2986);
    // ultimate apocalypse
    for(int i = 0; i < AVL_APOCALYPSE_ITER; i++)
    {
        run_case("test_apocalypse", test_apocalypse());
    }

    test_end();
}

