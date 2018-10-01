#include "test_main.h"
#include "test_case.h"
#include "lb.h"
#include <stdio.h>

struct test_node
{
    struct atree_node tree_entry;
    int32 val;
} test_node;

static struct test_node *
create_test_node(int val)
{
    struct test_node *rs = talloc(sizeof(struct test_node));
    rs->val = val;
    return rs;
}

static int32
test_node_compare(struct atree_node *root, struct atree_node *node)
{
    struct test_node *tree_node = OBTAIN_STRUCT_ADDR(root, struct test_node, tree_entry);
    struct test_node *self = OBTAIN_STRUCT_ADDR(node, struct test_node, tree_entry);
    return tree_node->val - self->val;
}

static void
_pre_order_print(struct atree_node *node)
{
    if (node == NULL)
    {
        return;
    }
    struct test_node *t_node = OBTAIN_STRUCT_ADDR(node, struct test_node, tree_entry);
    printf("%d-", t_node->val);
    _pre_order_print(node->left);
    _pre_order_print(node->right);
}

static void
pre_order(struct atree_node *node)
{
#ifdef TDBG
    _pre_order_print(node);
#endif
}

static bool
_pre_order_assert(struct atree_node *node, int order[], int size, int *counter)
{
    if (node == NULL)
    {
        return TRUE;
    }
    if (*counter >= size)
    {
        return FALSE;
    }

    bool result = TRUE;
    struct test_node *t_node = OBTAIN_STRUCT_ADDR(node, struct test_node, tree_entry);
    if (order[*counter] != t_node->val)
    {
        result = FALSE;
    }
    (*counter)++;
    result = result && _pre_order_assert(node->left, order, size, counter);
    result = result && _pre_order_assert(node->right, order, size, counter);
    return result;
}

static bool
pre_order_assert(struct atree *tree, int order[], int size)
{
    bool ret;
    int counter = 0;

    ret = _pre_order_assert(tree->root, order, size, &counter);
#ifdef TDBG
    if (!ret)
    {
        printf("[AVL ASSERT] Expected: ");
        for (int i = 0; i < size; i++)
        {
            printf("%d-", order[i]);
        }
        printf("\n             Got:");
        pre_order(tree->root);
        printf("\n");
        fflush(stdout);
    }
#endif
    return ret;
}

/**
 * Tests
 */

static bool
insert_simple_l(void)
{
    //1                   2
    // \                 / \
    //  2   == 1L ==>   1   3
    //   \
    //    3

    bool result = TRUE;
    struct atree tree;
    lb_atree_init(&tree, test_node_compare);

    lb_atree_insert(&tree, &create_test_node(1)->tree_entry);
    lb_atree_insert(&tree, &create_test_node(2)->tree_entry);
    int val1[] = {1, 2};
    result = result && pre_order_assert(&tree, val1, 2);

    lb_atree_insert(&tree, &create_test_node(3)->tree_entry);
    int val2[] = {2, 1, 3};
    result = result && pre_order_assert(&tree, val2, 3);
    return result && lb_atree_validate(&tree);
}

static bool
insert_simple_r(void)
{
    //    3               2
    //   /               / \
    //  2   == 1R ==>   1   3
    // /
    //1

    bool result = TRUE;
    struct atree tree;
    lb_atree_init(&tree, test_node_compare);

    lb_atree_insert(&tree, &create_test_node(3)->tree_entry);
    lb_atree_insert(&tree, &create_test_node(2)->tree_entry);
    int val1[] = {3, 2};
    result = result && pre_order_assert(&tree, val1, 2);

    lb_atree_insert(&tree, &create_test_node(1)->tree_entry);

    int val2[] = {2, 1, 3};
    result = result && pre_order_assert(&tree, val2, 3);
    return result && lb_atree_validate(&tree);
}

static bool
insert_simple_ll(void)
{
    //2                  3
    // \                / \
    //  4   == 2L ==>  2   4
    // /
    //3
    bool result = TRUE;
    struct atree tree;
    lb_atree_init(&tree, test_node_compare);

    lb_atree_insert(&tree, &create_test_node(2)->tree_entry);
    lb_atree_insert(&tree, &create_test_node(4)->tree_entry);
    int val1[] = {2, 4};
    result = result && pre_order_assert(&tree, val1, 2);

    lb_atree_insert(&tree, &create_test_node(3)->tree_entry);

    int val2[] = {3, 2, 4};
    result = result && pre_order_assert(&tree, val2, 3);
    return result && lb_atree_validate(&tree);
}

static bool
insert_simple_rr(void)
{
    //  4                3
    // /                / \
    //2     == 2R ==>  2   4
    // \
    //  3
    bool result = TRUE;
    struct atree tree;
    lb_atree_init(&tree, test_node_compare);

    lb_atree_insert(&tree, &create_test_node(4)->tree_entry);
    lb_atree_insert(&tree, &create_test_node(2)->tree_entry);
    int val1[] = {4, 2};
    result = result && pre_order_assert(&tree, val1, 2);

    lb_atree_insert(&tree, &create_test_node(3)->tree_entry);

    int val2[] = {3, 2, 4};
    result = result && pre_order_assert(&tree, val2, 3);
    return result && lb_atree_validate(&tree);
}

static bool
insert_complex_1(void)
{
    //    20+          20++           20++         9
    //   /  \         /  \           /  \         / \
    //  4    26 =>   4-   26 =>     9+   26 =>   4+  20
    // / \          / \            / \          /   /  \
    //3   9        3   9-         4+  15       3  15    26
    //                   \       /
    //                    15    3
    bool result = TRUE;
    struct atree tree;
    lb_atree_init(&tree, test_node_compare);

    lb_atree_insert(&tree, &create_test_node(20)->tree_entry);
    lb_atree_insert(&tree, &create_test_node(4)->tree_entry);
    lb_atree_insert(&tree, &create_test_node(26)->tree_entry);
    lb_atree_insert(&tree, &create_test_node(3)->tree_entry);
    lb_atree_insert(&tree, &create_test_node(9)->tree_entry);
    int val1[] = {20, 4, 3, 9, 26};
    result = result && pre_order_assert(&tree, val1, 5);

    lb_atree_insert(&tree, &create_test_node(15)->tree_entry);

    int val2[] = {9, 4, 3, 20, 15, 26};
    result = result && pre_order_assert(&tree, val2, 6);
    return result && lb_atree_validate(&tree);
}

static bool
insert_complex_2(void)
{
    //    20+          20++           20++         9
    //   /  \         /  \           /  \         / \
    //  4    26 =>   4-   26 =>     9++  26 =>   4   20-
    // / \          / \            /            / \    \
    //3   9        3   9+         4            3   8    26
    //                /          / \
    //               8          3   8
    bool result = TRUE;
    struct atree tree;
    lb_atree_init(&tree, test_node_compare);

    lb_atree_insert(&tree, &create_test_node(20)->tree_entry);
    lb_atree_insert(&tree, &create_test_node(4)->tree_entry);
    lb_atree_insert(&tree, &create_test_node(26)->tree_entry);
    lb_atree_insert(&tree, &create_test_node(3)->tree_entry);
    lb_atree_insert(&tree, &create_test_node(9)->tree_entry);
    int val1[] = {20, 4, 3, 9, 26};
    result = result && pre_order_assert(&tree, val1, 5);

    lb_atree_insert(&tree, &create_test_node(8)->tree_entry);

    int val2[] = {9, 4, 3, 8, 20, 26};
    result = result && pre_order_assert(&tree, val2, 6);
    return result && lb_atree_validate(&tree);
}

static bool
insert_complex_3(void)
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
    bool result = TRUE;
    struct atree tree;
    lb_atree_init(&tree, test_node_compare);

    lb_atree_insert(&tree, &create_test_node(20)->tree_entry);
    lb_atree_insert(&tree, &create_test_node(4)->tree_entry);
    lb_atree_insert(&tree, &create_test_node(26)->tree_entry);
    lb_atree_insert(&tree, &create_test_node(3)->tree_entry);
    lb_atree_insert(&tree, &create_test_node(9)->tree_entry);
    lb_atree_insert(&tree, &create_test_node(21)->tree_entry);
    lb_atree_insert(&tree, &create_test_node(30)->tree_entry);
    lb_atree_insert(&tree, &create_test_node(2)->tree_entry);
    lb_atree_insert(&tree, &create_test_node(7)->tree_entry);
    lb_atree_insert(&tree, &create_test_node(11)->tree_entry);
    int val1[] = {20, 4, 3, 2, 9, 7, 11, 26, 21, 30};
    result = result && pre_order_assert(&tree, val1, 10);

    lb_atree_insert(&tree, &create_test_node(15)->tree_entry);

    int val2[] = {9, 4, 3, 2, 7, 20, 11, 15, 26, 21, 30};
    result = result && pre_order_assert(&tree, val2, 11);
    return result && lb_atree_validate(&tree);
}

static bool
insert_complex_4(void)
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
    bool result = TRUE;
    struct atree tree;
    lb_atree_init(&tree, test_node_compare);

    lb_atree_insert(&tree, &create_test_node(20)->tree_entry);
    lb_atree_insert(&tree, &create_test_node(4)->tree_entry);
    lb_atree_insert(&tree, &create_test_node(26)->tree_entry);
    lb_atree_insert(&tree, &create_test_node(3)->tree_entry);
    lb_atree_insert(&tree, &create_test_node(9)->tree_entry);
    lb_atree_insert(&tree, &create_test_node(21)->tree_entry);
    lb_atree_insert(&tree, &create_test_node(30)->tree_entry);
    lb_atree_insert(&tree, &create_test_node(2)->tree_entry);
    lb_atree_insert(&tree, &create_test_node(7)->tree_entry);
    lb_atree_insert(&tree, &create_test_node(11)->tree_entry);
    int val1[] = {20, 4, 3, 2, 9, 7, 11, 26, 21, 30};
    result = result && pre_order_assert(&tree, val1, 10);

    lb_atree_insert(&tree, &create_test_node(8)->tree_entry);

    int val2[] = {9, 4, 3, 2, 7, 8, 20, 11, 26, 21, 30};
    result = result && pre_order_assert(&tree, val2, 11);
    return result && lb_atree_validate(&tree);
}

static bool
insert_duplicate(void)
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
    bool result = TRUE;
    struct atree tree;
    struct test_node *temp, *temp20, *temp30, *temp7, *temp2;
    lb_atree_init(&tree, test_node_compare);

    temp20 = create_test_node(20);
    temp30 = create_test_node(30);
    temp7 = create_test_node(7);
    temp2 = create_test_node(2);

    lb_atree_insert(&tree, &temp20->tree_entry);
    lb_atree_insert(&tree, &create_test_node(4)->tree_entry);
    lb_atree_insert(&tree, &create_test_node(26)->tree_entry);
    lb_atree_insert(&tree, &create_test_node(3)->tree_entry);
    lb_atree_insert(&tree, &create_test_node(9)->tree_entry);
    lb_atree_insert(&tree, &create_test_node(21)->tree_entry);
    lb_atree_insert(&tree, &temp30->tree_entry);
    lb_atree_insert(&tree, &temp2->tree_entry);
    lb_atree_insert(&tree, &temp7->tree_entry);
    lb_atree_insert(&tree, &create_test_node(11)->tree_entry);
    int val1[] = {20, 4, 3, 2, 9, 7, 11, 26, 21, 30};
    result = result && pre_order_assert(&tree, val1, 10);

    // should return the value being overwritten
    temp = OBTAIN_STRUCT_ADDR(lb_atree_insert(&tree, &create_test_node(20)->tree_entry), struct test_node, tree_entry);
    result = result && (temp == temp20);
    temp = OBTAIN_STRUCT_ADDR(lb_atree_insert(&tree, &create_test_node(30)->tree_entry), struct test_node, tree_entry);
    result = result && (temp == temp30);
    temp = OBTAIN_STRUCT_ADDR(lb_atree_insert(&tree, &create_test_node(7)->tree_entry), struct test_node, tree_entry);
    result = result && (temp == temp7);
    temp = OBTAIN_STRUCT_ADDR(lb_atree_insert(&tree, &create_test_node(2)->tree_entry), struct test_node, tree_entry);
    result = result && (temp == temp2);

    result = result && pre_order_assert(&tree, val1, 10);
    return result && lb_atree_validate(&tree);
}


static bool
delete_simple_l(void)
{
    //  2                   3
    // x \                 / \
    //1   3   == 1L ==>   2   4
    //     \
    //      4

    bool result = TRUE;
    struct atree tree;
    lb_atree_init(&tree, test_node_compare);

    struct test_node *deleted = create_test_node(1);

    lb_atree_insert(&tree, &create_test_node(2)->tree_entry);
    lb_atree_insert(&tree, &create_test_node(3)->tree_entry);
    lb_atree_insert(&tree, &deleted->tree_entry);
    lb_atree_insert(&tree, &create_test_node(4)->tree_entry);
    int val1[] = {2, 1, 3, 4};
    result = result && pre_order_assert(&tree, val1, 4);

    lb_atree_delete(&tree, &deleted->tree_entry);

    int val2[] = {3, 2, 4};
    result = result && pre_order_assert(&tree, val2, 3);
    return result && lb_atree_validate(&tree);
}

static bool
delete_simple_r(void)
{
    //    3                  2
    //   / x                / \
    //  2   4  == 1R ==>   1   3
    // /
    //1

    bool result = TRUE;
    struct atree tree;
    lb_atree_init(&tree, test_node_compare);

    struct test_node *deleted = create_test_node(4);

    lb_atree_insert(&tree, &create_test_node(3)->tree_entry);
    lb_atree_insert(&tree, &create_test_node(2)->tree_entry);
    lb_atree_insert(&tree, &deleted->tree_entry);
    lb_atree_insert(&tree, &create_test_node(1)->tree_entry);
    int val1[] = {3, 2, 1, 4};
    result = result && pre_order_assert(&tree, val1, 4);

    lb_atree_delete(&tree, &deleted->tree_entry);

    int val2[] = {2, 1, 3};
    result = result && pre_order_assert(&tree, val2, 3);
    return result && lb_atree_validate(&tree);
}

static bool
delete_simple_ll(void)
{
    //  2                  3
    // x \                / \
    //1   4   == 2L ==>  2   4
    //   /
    //  3
    bool result = TRUE;
    struct atree tree;
    lb_atree_init(&tree, test_node_compare);

    struct test_node *deleted = create_test_node(1);

    lb_atree_insert(&tree, &create_test_node(2)->tree_entry);
    lb_atree_insert(&tree, &create_test_node(4)->tree_entry);
    lb_atree_insert(&tree, &deleted->tree_entry);
    lb_atree_insert(&tree, &create_test_node(3)->tree_entry);
    int val1[] = {2, 1, 4, 3};
    result = result && pre_order_assert(&tree, val1, 4);

    lb_atree_delete(&tree, &deleted->tree_entry);

    int val2[] = {3, 2, 4};
    result = result && pre_order_assert(&tree, val2, 3);
    return result && lb_atree_validate(&tree);
}

static bool
delete_simple_rr(void)
{
    //  3                  2
    // / x                / \
    //2   4   == 2R ==>  1   3
    // \
    //  1
    bool result = TRUE;
    struct atree tree;
    lb_atree_init(&tree, test_node_compare);

    struct test_node *deleted = create_test_node(4);

    lb_atree_insert(&tree, &create_test_node(3)->tree_entry);
    lb_atree_insert(&tree, &create_test_node(2)->tree_entry);
    lb_atree_insert(&tree, &deleted->tree_entry);
    lb_atree_insert(&tree, &create_test_node(1)->tree_entry);
    int val1[] = {3, 2, 1, 4};
    result = result && pre_order_assert(&tree, val1, 4);

    lb_atree_delete(&tree, &deleted->tree_entry);

    int val2[] = {2, 1, 3};
    result = result && pre_order_assert(&tree, val2, 3);
    return result && lb_atree_validate(&tree);
}

static bool
delete_complex_1(void)
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
    bool result = TRUE;
    struct atree tree;
    lb_atree_init(&tree, test_node_compare);

    struct test_node *deleted = create_test_node(10);

    lb_atree_insert(&tree, &deleted->tree_entry);
    int val1[] = {10};
    result = result && pre_order_assert(&tree, val1, 1);

    lb_atree_delete(&tree, &deleted->tree_entry);

    result = result && pre_order_assert(&tree, val1, 0);
    return result && lb_atree_validate(&tree);
}

static bool
delete_complex_2(void)
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
    //          (replace root with smallest value on the treenode or 25)
    //
    //                     25
    //                    /  \
	//                  10    30
    //                         \
	//                         35
    //
    bool result = TRUE;
    struct atree tree;
    lb_atree_init(&tree, test_node_compare);

    struct test_node *deleted = create_test_node(20);

    lb_atree_insert(&tree, &deleted->tree_entry);
    lb_atree_insert(&tree, &create_test_node(10)->tree_entry);
    lb_atree_insert(&tree, &create_test_node(30)->tree_entry);
    lb_atree_insert(&tree, &create_test_node(25)->tree_entry);
    lb_atree_insert(&tree, &create_test_node(35)->tree_entry);
    int val1[] = {20, 10, 30, 25, 35};
    result = result && pre_order_assert(&tree, val1, 5);

    lb_atree_delete(&tree, &deleted->tree_entry);

    int val2[] = {25, 10, 30, 35};
    result = result && pre_order_assert(&tree, val2, 4);
    return result && lb_atree_validate(&tree);
}

static bool
delete_complex_3(void)
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
    bool result = TRUE;
    struct atree tree;
    lb_atree_init(&tree, test_node_compare);

    struct test_node *deleted = create_test_node(10);

    lb_atree_insert(&tree, &create_test_node(20)->tree_entry);
    lb_atree_insert(&tree, &deleted->tree_entry);
    lb_atree_insert(&tree, &create_test_node(30)->tree_entry);
    lb_atree_insert(&tree, &create_test_node(5)->tree_entry);
    lb_atree_insert(&tree, &create_test_node(15)->tree_entry);
    lb_atree_insert(&tree, &create_test_node(25)->tree_entry);
    int val1[] = {20, 10, 5, 15, 30, 25};
    result = result && pre_order_assert(&tree, val1, 6);

    lb_atree_delete(&tree, &deleted->tree_entry);

    int val2[] = {20, 15, 5, 30, 25};
    result = result && pre_order_assert(&tree, val2, 5);
    return result && lb_atree_validate(&tree);
}

static bool
delete_complex_4(void)
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
    bool result = TRUE;
    struct atree tree;
    lb_atree_init(&tree, test_node_compare);

    struct test_node *delete5 = create_test_node(5);
    struct test_node *delete10 = create_test_node(10);
    struct test_node *delete15 = create_test_node(15);
    struct test_node *delete25 = create_test_node(25);
    struct test_node *delete30 = create_test_node(30);


    lb_atree_insert(&tree, &create_test_node(20)->tree_entry);
    lb_atree_insert(&tree, &delete10->tree_entry);
    lb_atree_insert(&tree, &delete30->tree_entry);
    lb_atree_insert(&tree, &delete5->tree_entry);
    lb_atree_insert(&tree, &delete15->tree_entry);
    lb_atree_insert(&tree, &delete25->tree_entry);

    int val1[] = {20, 10, 5, 15, 30, 25};
    result = result && pre_order_assert(&tree, val1, 6);

    lb_atree_delete(&tree, &delete5->tree_entry);
    lb_atree_delete(&tree, &delete15->tree_entry);
    lb_atree_delete(&tree, &delete25->tree_entry);
    lb_atree_delete(&tree, &delete10->tree_entry);
    lb_atree_delete(&tree, &delete30->tree_entry);

    int val2[] = {20};
    result = result && pre_order_assert(&tree, val2, 1);
    return result && lb_atree_validate(&tree);
}

static bool
delete_complex_single_rotation(void)
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
    bool result = TRUE;
    struct atree tree;
    lb_atree_init(&tree, test_node_compare);

    struct test_node *deleted = create_test_node(50);

    lb_atree_insert(&tree, &create_test_node(20)->tree_entry);
    lb_atree_insert(&tree, &create_test_node(10)->tree_entry);
    lb_atree_insert(&tree, &create_test_node(30)->tree_entry);
    lb_atree_insert(&tree, &create_test_node(5)->tree_entry);
    lb_atree_insert(&tree, &create_test_node(15)->tree_entry);
    lb_atree_insert(&tree, &create_test_node(25)->tree_entry);
    lb_atree_insert(&tree, &create_test_node(40)->tree_entry);
    lb_atree_insert(&tree, &create_test_node(12)->tree_entry);
    lb_atree_insert(&tree, &create_test_node(22)->tree_entry);
    lb_atree_insert(&tree, &create_test_node(35)->tree_entry);
    lb_atree_insert(&tree, &deleted->tree_entry);
    lb_atree_insert(&tree, &create_test_node(31)->tree_entry);
    int val1[] = {20, 10, 5, 15, 12, 30, 25, 22, 40, 35, 31, 50};
    result = result && pre_order_assert(&tree, val1, 12);

    lb_atree_delete(&tree, &deleted->tree_entry);

    int val2[] = {20, 10, 5, 15, 12, 30, 25, 22, 35, 31, 40};
    result = result && pre_order_assert(&tree, val2, 11);
    return result && lb_atree_validate(&tree);
}

static bool
delete_complex_double_rotation(void)
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
    bool result = TRUE;
    struct atree tree;
    lb_atree_init(&tree, test_node_compare);

    struct test_node *deleted = create_test_node(22);

    lb_atree_insert(&tree, &create_test_node(20)->tree_entry);
    lb_atree_insert(&tree, &create_test_node(10)->tree_entry);
    lb_atree_insert(&tree, &create_test_node(30)->tree_entry);
    lb_atree_insert(&tree, &create_test_node(5)->tree_entry);
    lb_atree_insert(&tree, &create_test_node(15)->tree_entry);
    lb_atree_insert(&tree, &create_test_node(25)->tree_entry);
    lb_atree_insert(&tree, &create_test_node(40)->tree_entry);
    lb_atree_insert(&tree, &create_test_node(12)->tree_entry);
    lb_atree_insert(&tree, &deleted->tree_entry);
    lb_atree_insert(&tree, &create_test_node(35)->tree_entry);
    lb_atree_insert(&tree, &create_test_node(50)->tree_entry);
    lb_atree_insert(&tree, &create_test_node(31)->tree_entry);
    int val1[] = {20, 10, 5, 15, 12, 30, 25, 22, 40, 35, 31, 50};
    result = result && pre_order_assert(&tree, val1, 12);

    lb_atree_delete(&tree, &deleted->tree_entry);

    int val2[] = {20, 10, 5, 15, 12, 35, 30, 25, 31, 40, 50};
    result = result && pre_order_assert(&tree, val2, 11);
    return result && lb_atree_validate(&tree);
}

static bool
delete_complex_multiple_rotation(void)
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
    bool result = TRUE;
    struct atree tree;
    lb_atree_init(&tree, test_node_compare);

    struct test_node *deleted = create_test_node(5);

    lb_atree_insert(&tree, &create_test_node(20)->tree_entry);
    lb_atree_insert(&tree, &create_test_node(10)->tree_entry);
    lb_atree_insert(&tree, &create_test_node(30)->tree_entry);
    lb_atree_insert(&tree, &deleted->tree_entry);
    lb_atree_insert(&tree, &create_test_node(15)->tree_entry);
    lb_atree_insert(&tree, &create_test_node(25)->tree_entry);
    lb_atree_insert(&tree, &create_test_node(40)->tree_entry);
    lb_atree_insert(&tree, &create_test_node(12)->tree_entry);
    lb_atree_insert(&tree, &create_test_node(22)->tree_entry);
    lb_atree_insert(&tree, &create_test_node(35)->tree_entry);
    lb_atree_insert(&tree, &create_test_node(50)->tree_entry);
    lb_atree_insert(&tree, &create_test_node(31)->tree_entry);
    int val1[] = {20, 10, 5, 15, 12, 30, 25, 22, 40, 35, 31, 50};
    result = result && pre_order_assert(&tree, val1, 12);

    lb_atree_delete(&tree, &deleted->tree_entry);

    int val2[] = {30, 20, 12, 10, 15, 25, 22, 40, 35, 31, 50};
    result = result && pre_order_assert(&tree, val2, 11);
    return result && lb_atree_validate(&tree);
}

static bool
delete_DNE(void)
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
    bool result = TRUE;
    struct atree tree;
    lb_atree_init(&tree, test_node_compare);

    struct test_node *delete100 = create_test_node(100);
    struct test_node *delete24 = create_test_node(24);

    lb_atree_insert(&tree, &create_test_node(20)->tree_entry);
    lb_atree_insert(&tree, &create_test_node(10)->tree_entry);
    lb_atree_insert(&tree, &create_test_node(30)->tree_entry);
    lb_atree_insert(&tree, &create_test_node(5)->tree_entry);
    lb_atree_insert(&tree, &create_test_node(15)->tree_entry);
    lb_atree_insert(&tree, &create_test_node(25)->tree_entry);

    int val1[] = {20, 10, 5, 15, 30, 25};
    result = result && pre_order_assert(&tree, val1, 6);

    lb_atree_delete(&tree, &delete24->tree_entry);
    lb_atree_delete(&tree, &delete100->tree_entry);
    result = result && pre_order_assert(&tree, val1, 6);
    return result && lb_atree_validate(&tree);
}

#define AVL_APOCALYPSE_NUM 1000
#define AVL_APOCALYPSE_IT 5
static struct test_node apocalypse[AVL_APOCALYPSE_NUM];

static bool
test_apocalypse(void)
{
    bool result = TRUE;
    struct atree tree;
    lb_atree_init(&tree, test_node_compare);

    // insert test
    for (int i = 0; i < AVL_APOCALYPSE_NUM; i++)
    {
        apocalypse[i].val = (int32) lb_rand();
        while (lb_atree_search(&tree, &apocalypse[i].tree_entry) != NULL)
        {
            apocalypse[i].val += (int32) lb_rand() % 32765;
        }
        lb_atree_insert(&tree, &apocalypse[i].tree_entry);
    }

    // integrity test
    result = result && lb_atree_validate(&tree);
    result = result && lb_atree_size(&tree) == AVL_APOCALYPSE_NUM;

    // smaller and bigger test
    struct atree_node *entry = lb_atree_min(&tree);
    uint32 size = 0;
    int32 prev = -1;
    int32 cur = OBTAIN_STRUCT_ADDR(entry, struct test_node, tree_entry)->val;
    while (entry != NULL)
    {
        if (cur < prev)
        {
            result = FALSE;
            break;
        }
        size++;
        entry = lb_atree_next(&tree, entry);
        prev = cur;
        if (entry != NULL)
        {
            cur = OBTAIN_STRUCT_ADDR(entry, struct test_node, tree_entry)->val;
        }
    }

    result = result && size == AVL_APOCALYPSE_NUM;

    // larger test
    entry = lb_atree_max(&tree);
    size = 0;
    cur = OBTAIN_STRUCT_ADDR(entry, struct test_node, tree_entry)->val;
    prev = cur;
    while (entry != NULL)
    {
        if (cur > prev)
        {
            result = FALSE;
            break;
        }
        size++;
        entry = lb_atree_prev(&tree, entry);
        prev = cur;
        if (entry != NULL)
        {
            cur = OBTAIN_STRUCT_ADDR(entry, struct test_node, tree_entry)->val;
        }
    }

    result = result && size == AVL_APOCALYPSE_NUM;


    // delete and search test
    for (int i = 0; i < AVL_APOCALYPSE_NUM; i++)
    {
        result = result && (lb_atree_search(&tree, &apocalypse[i].tree_entry) != NULL);
        lb_atree_delete(&tree, &apocalypse[i].tree_entry);
        result = result && (lb_atree_search(&tree, &apocalypse[i].tree_entry) == NULL);
        result = result && lb_atree_validate(&tree);
    }

    result = result && (lb_atree_size(&tree) == 0);
    return result;
}


void
avl_tree_test(void)
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

    lb_srand(2986);
    // ultimate apocalypse
    for (int i = 0; i < AVL_APOCALYPSE_IT; i++)
    {
        run_case("test_apocalypse", test_apocalypse());
    }

    test_end();
}

