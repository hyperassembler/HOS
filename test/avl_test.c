#include <common/cdef.h>
#include <common/libkern.h>
#include <common/avl_tree.h>
#include <test/ktest.h>
#include <ke/brute.h>

struct test_node {
    struct avl_node tree_entry;
    int32 val;
} test_node;

#define AVL_BRUTE_TEST_NODE 1024
#define AVL_MAX_TEST_NODE (AVL_BRUTE_TEST_NODE + 512)

static struct test_node avl_alloc_nodes[AVL_MAX_TEST_NODE];
static int avl_alloc_idx = 0;

static struct test_node *
create_test_node(int val)
{
    KASSERT(avl_alloc_idx < AVL_MAX_TEST_NODE, "node allocation overflow");

    struct test_node *rs = &avl_alloc_nodes[avl_alloc_idx++];
    rs->val = val;
    return rs;
}

static int
test_node_compare(struct avl_node *root, struct avl_node *node)
{
    struct test_node *tree_node = OBTAIN_STRUCT_ADDR(root, struct test_node, tree_entry);
    struct test_node *self = OBTAIN_STRUCT_ADDR(node, struct test_node, tree_entry);
    return tree_node->val - self->val;
}

static void
pre_order_print(struct avl_node *node)
{
    if (node == NULL) {
        return;
    }
    struct test_node *t_node = OBTAIN_STRUCT_ADDR(node, struct test_node, tree_entry);
    kprintf("%d-", t_node->val);
    pre_order_print(node->left);
    pre_order_print(node->right);
}

static void ATTR_UNUSED
pre_order(struct avl_node *node)
{
    pre_order_print(node);
}

static int
_pre_order_assert(struct avl_node *node, const int *order, int size, int *counter)
{
    if (node == NULL) {
        return 1;
    }
    if (*counter >= size) {
        return 0;
    }

    int result = 1;
    struct test_node *t_node = OBTAIN_STRUCT_ADDR(node, struct test_node, tree_entry);
    if (order[*counter] != t_node->val) {
        result = 0;
    }
    (*counter)++;
    result = result && _pre_order_assert(node->left, order, size, counter);
    result = result && _pre_order_assert(node->right, order, size, counter);
    return result;
}

static int
pre_order_assert(struct avl_root *tree, int order[], int size)
{
    int ret;
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

static void
insert_simple_l(void)
{
    //1                   2
    // \                 / \
    //  2   == 1L ==>   1   3
    //   \
    //    3
    ktest_begin("insert_simple_l");
    struct avl_root tree;
    avl_init(&tree, test_node_compare);

    avl_insert(&tree, &create_test_node(1)->tree_entry);
    avl_insert(&tree, &create_test_node(2)->tree_entry);
    int val1[] = {1, 2};
    KASSERT(pre_order_assert(&tree, val1, 2), "insert_simple_l_1");

    avl_insert(&tree, &create_test_node(3)->tree_entry);
    int val2[] = {2, 1, 3};
    KASSERT(pre_order_assert(&tree, val2, 3), "insert_simple_l_2");
    KASSERT(avl_validate(&tree), "validate");
}

static void
insert_simple_r(void)
{
    //    3               2
    //   /               / \
    //  2   == 1R ==>   1   3
    // /
    //1
    ktest_begin("insert_simple_r");
    struct avl_root tree;
    avl_init(&tree, test_node_compare);

    avl_insert(&tree, &create_test_node(3)->tree_entry);
    avl_insert(&tree, &create_test_node(2)->tree_entry);
    int val1[] = {3, 2};
    KASSERT(pre_order_assert(&tree, val1, 2), "insert_simple_r_1");

    avl_insert(&tree, &create_test_node(1)->tree_entry);

    int val2[] = {2, 1, 3};
    KASSERT(pre_order_assert(&tree, val2, 3), "insert_simple_r_2");
    KASSERT(avl_validate(&tree), "validate");
}

static void
insert_simple_ll(void)
{
    //2                  3
    // \                / \
    //  4   == 2L ==>  2   4
    // /
    //3
    ktest_begin("insert_simple_ll");
    struct avl_root tree;
    avl_init(&tree, test_node_compare);

    avl_insert(&tree, &create_test_node(2)->tree_entry);
    avl_insert(&tree, &create_test_node(4)->tree_entry);
    int val1[] = {2, 4};
    KASSERT(pre_order_assert(&tree, val1, 2), "insert_simple_ll_1");

    avl_insert(&tree, &create_test_node(3)->tree_entry);

    int val2[] = {3, 2, 4};
    KASSERT(pre_order_assert(&tree, val2, 3), "insert_simple_ll_2");
    KASSERT(avl_validate(&tree), "validate");
}

static void
insert_simple_rr(void)
{
    //  4                3
    // /                / \
    //2     == 2R ==>  2   4
    // \
    //  3
    ktest_begin("insert_simple_rr");
    struct avl_root tree;
    avl_init(&tree, test_node_compare);

    avl_insert(&tree, &create_test_node(4)->tree_entry);
    avl_insert(&tree, &create_test_node(2)->tree_entry);
    int val1[] = {4, 2};
    KASSERT(pre_order_assert(&tree, val1, 2), "insert_simple_rr_1");

    avl_insert(&tree, &create_test_node(3)->tree_entry);

    int val2[] = {3, 2, 4};
    KASSERT(pre_order_assert(&tree, val2, 3), "insert_simple_rr_2");
    KASSERT(avl_validate(&tree), "validate");
}

static void
insert_complex_1(void)
{
    //    20+          20++           20++         9
    //   /  \         /  \           /  \         / \
    //  4    26 =>   4-   26 =>     9+   26 =>   4+  20
    // / \          / \            / \          /   /  \
    //3   9        3   9-         4+  15       3  15    26
    //                   \       /
    //                    15    3
    ktest_begin("insert_complex_1");
    struct avl_root tree;
    avl_init(&tree, test_node_compare);

    avl_insert(&tree, &create_test_node(20)->tree_entry);
    avl_insert(&tree, &create_test_node(4)->tree_entry);
    avl_insert(&tree, &create_test_node(26)->tree_entry);
    avl_insert(&tree, &create_test_node(3)->tree_entry);
    avl_insert(&tree, &create_test_node(9)->tree_entry);
    int val1[] = {20, 4, 3, 9, 26};
    KASSERT(pre_order_assert(&tree, val1, 5), "insert_complex_1_1");

    avl_insert(&tree, &create_test_node(15)->tree_entry);

    int val2[] = {9, 4, 3, 20, 15, 26};
    KASSERT(pre_order_assert(&tree, val2, 6), "insert_complex_1_2");
    KASSERT(avl_validate(&tree), "validate");
}

static void
insert_complex_2(void)
{
    //    20+          20++           20++         9
    //   /  \         /  \           /  \         / \
    //  4    26 =>   4-   26 =>     9++  26 =>   4   20-
    // / \          / \            /            / \    \
    //3   9        3   9+         4            3   8    26
    //                /          / \
    //               8          3   8
    ktest_begin("insert_complex_2");
    struct avl_root tree;
    avl_init(&tree, test_node_compare);

    avl_insert(&tree, &create_test_node(20)->tree_entry);
    avl_insert(&tree, &create_test_node(4)->tree_entry);
    avl_insert(&tree, &create_test_node(26)->tree_entry);
    avl_insert(&tree, &create_test_node(3)->tree_entry);
    avl_insert(&tree, &create_test_node(9)->tree_entry);
    int val1[] = {20, 4, 3, 9, 26};
    KASSERT(pre_order_assert(&tree, val1, 5), "insert_complex_2_1");

    avl_insert(&tree, &create_test_node(8)->tree_entry);

    int val2[] = {9, 4, 3, 8, 20, 26};
    KASSERT(pre_order_assert(&tree, val2, 6), "insert_complex_2_2");
    KASSERT(avl_validate(&tree), "validate");
}

static void
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
    ktest_begin("insert_complex_3");
    struct avl_root tree;
    avl_init(&tree, test_node_compare);

    avl_insert(&tree, &create_test_node(20)->tree_entry);
    avl_insert(&tree, &create_test_node(4)->tree_entry);
    avl_insert(&tree, &create_test_node(26)->tree_entry);
    avl_insert(&tree, &create_test_node(3)->tree_entry);
    avl_insert(&tree, &create_test_node(9)->tree_entry);
    avl_insert(&tree, &create_test_node(21)->tree_entry);
    avl_insert(&tree, &create_test_node(30)->tree_entry);
    avl_insert(&tree, &create_test_node(2)->tree_entry);
    avl_insert(&tree, &create_test_node(7)->tree_entry);
    avl_insert(&tree, &create_test_node(11)->tree_entry);
    int val1[] = {20, 4, 3, 2, 9, 7, 11, 26, 21, 30};
    KASSERT(pre_order_assert(&tree, val1, 10), "insert_complex_3_1");

    avl_insert(&tree, &create_test_node(15)->tree_entry);

    int val2[] = {9, 4, 3, 2, 7, 20, 11, 15, 26, 21, 30};
    KASSERT(pre_order_assert(&tree, val2, 11), "insert_complex_3_2");
    KASSERT(avl_validate(&tree), "validate");
}

static void
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
    ktest_begin("insert_complex_4");
    struct avl_root tree;
    avl_init(&tree, test_node_compare);

    avl_insert(&tree, &create_test_node(20)->tree_entry);
    avl_insert(&tree, &create_test_node(4)->tree_entry);
    avl_insert(&tree, &create_test_node(26)->tree_entry);
    avl_insert(&tree, &create_test_node(3)->tree_entry);
    avl_insert(&tree, &create_test_node(9)->tree_entry);
    avl_insert(&tree, &create_test_node(21)->tree_entry);
    avl_insert(&tree, &create_test_node(30)->tree_entry);
    avl_insert(&tree, &create_test_node(2)->tree_entry);
    avl_insert(&tree, &create_test_node(7)->tree_entry);
    avl_insert(&tree, &create_test_node(11)->tree_entry);
    int val1[] = {20, 4, 3, 2, 9, 7, 11, 26, 21, 30};
    KASSERT(pre_order_assert(&tree, val1, 10), "insert_complex_4_1");

    avl_insert(&tree, &create_test_node(8)->tree_entry);

    int val2[] = {9, 4, 3, 2, 7, 8, 20, 11, 26, 21, 30};
    KASSERT(pre_order_assert(&tree, val2, 11), "insert_complex_4_2");
    KASSERT(avl_validate(&tree), "validate");
}

static void
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
    ktest_begin("insert_duplicate");
    struct avl_root tree;
    struct test_node *temp, *temp20, *temp30, *temp7, *temp2;
    avl_init(&tree, test_node_compare);

    temp20 = create_test_node(20);
    temp30 = create_test_node(30);
    temp7 = create_test_node(7);
    temp2 = create_test_node(2);

    avl_insert(&tree, &temp20->tree_entry);
    avl_insert(&tree, &create_test_node(4)->tree_entry);
    avl_insert(&tree, &create_test_node(26)->tree_entry);
    avl_insert(&tree, &create_test_node(3)->tree_entry);
    avl_insert(&tree, &create_test_node(9)->tree_entry);
    avl_insert(&tree, &create_test_node(21)->tree_entry);
    avl_insert(&tree, &temp30->tree_entry);
    avl_insert(&tree, &temp2->tree_entry);
    avl_insert(&tree, &temp7->tree_entry);
    avl_insert(&tree, &create_test_node(11)->tree_entry);
    int val1[] = {20, 4, 3, 2, 9, 7, 11, 26, 21, 30};
    KASSERT(pre_order_assert(&tree, val1, 10), "insert_duplicate_1");

    // should return the value being overwritten
    temp = OBTAIN_STRUCT_ADDR(avl_insert(&tree, &create_test_node(20)->tree_entry), struct test_node, tree_entry);
    KASSERT((temp == temp20), "insert_duplicate_2");
    temp = OBTAIN_STRUCT_ADDR(avl_insert(&tree, &create_test_node(30)->tree_entry), struct test_node, tree_entry);
    KASSERT((temp == temp30), "insert_duplicate_3");
    temp = OBTAIN_STRUCT_ADDR(avl_insert(&tree, &create_test_node(7)->tree_entry), struct test_node, tree_entry);
    KASSERT((temp == temp7), "insert_duplicate_4");
    temp = OBTAIN_STRUCT_ADDR(avl_insert(&tree, &create_test_node(2)->tree_entry), struct test_node, tree_entry);
    KASSERT((temp == temp2), "insert_duplicate_5");

    KASSERT(pre_order_assert(&tree, val1, 10), "insert_duplicate_6");
    KASSERT(avl_validate(&tree), "validate");
}


static void
delete_simple_l(void)
{
    //  2                   3
    // x \                 / \
    //1   3   == 1L ==>   2   4
    //     \
    //      4
    ktest_begin("delete_simple_l");
    struct avl_root tree;
    avl_init(&tree, test_node_compare);

    struct test_node *deleted = create_test_node(1);

    avl_insert(&tree, &create_test_node(2)->tree_entry);
    avl_insert(&tree, &create_test_node(3)->tree_entry);
    avl_insert(&tree, &deleted->tree_entry);
    avl_insert(&tree, &create_test_node(4)->tree_entry);
    int val1[] = {2, 1, 3, 4};
    KASSERT(pre_order_assert(&tree, val1, 4), "delete_simple_l_1");

    avl_remove(&tree, &deleted->tree_entry);

    int val2[] = {3, 2, 4};
    KASSERT(pre_order_assert(&tree, val2, 3), "delete_simple_l_2");
    KASSERT(avl_validate(&tree), "validate");
}

static void
delete_simple_r(void)
{
    //    3                  2
    //   / x                / \
    //  2   4  == 1R ==>   1   3
    // /
    //1
    ktest_begin("delete_simple_r");
    struct avl_root tree;
    avl_init(&tree, test_node_compare);

    struct test_node *deleted = create_test_node(4);

    avl_insert(&tree, &create_test_node(3)->tree_entry);
    avl_insert(&tree, &create_test_node(2)->tree_entry);
    avl_insert(&tree, &deleted->tree_entry);
    avl_insert(&tree, &create_test_node(1)->tree_entry);
    int val1[] = {3, 2, 1, 4};
    KASSERT(pre_order_assert(&tree, val1, 4), "delete_simple_r_1");

    avl_remove(&tree, &deleted->tree_entry);

    int val2[] = {2, 1, 3};
    KASSERT(pre_order_assert(&tree, val2, 3), "delete_simple_r_2");
    KASSERT(avl_validate(&tree), "validate");
}

static void
delete_simple_ll(void)
{
    //  2                  3
    // x \                / \
    //1   4   == 2L ==>  2   4
    //   /
    //  3
    ktest_begin("delete_simple_ll");
    struct avl_root tree;
    avl_init(&tree, test_node_compare);

    struct test_node *deleted = create_test_node(1);

    avl_insert(&tree, &create_test_node(2)->tree_entry);
    avl_insert(&tree, &create_test_node(4)->tree_entry);
    avl_insert(&tree, &deleted->tree_entry);
    avl_insert(&tree, &create_test_node(3)->tree_entry);
    int val1[] = {2, 1, 4, 3};
    KASSERT(pre_order_assert(&tree, val1, 4), "delete_simple_ll_1");

    avl_remove(&tree, &deleted->tree_entry);

    int val2[] = {3, 2, 4};
    KASSERT(pre_order_assert(&tree, val2, 3), "delete_simple_ll_2");
    KASSERT(avl_validate(&tree), "validate");
}

static void
delete_simple_rr(void)
{
    //  3                  2
    // / x                / \
    //2   4   == 2R ==>  1   3
    // \
    //  1
    ktest_begin("delete_simple_rr");
    struct avl_root tree;
    avl_init(&tree, test_node_compare);

    struct test_node *deleted = create_test_node(4);

    avl_insert(&tree, &create_test_node(3)->tree_entry);
    avl_insert(&tree, &create_test_node(2)->tree_entry);
    avl_insert(&tree, &deleted->tree_entry);
    avl_insert(&tree, &create_test_node(1)->tree_entry);
    int val1[] = {3, 2, 1, 4};
    KASSERT(pre_order_assert(&tree, val1, 4), "delete_simple_rr_1" );

    avl_remove(&tree, &deleted->tree_entry);

    int val2[] = {2, 1, 3};
    KASSERT(pre_order_assert(&tree, val2, 3), "delete_simple_rr_2" );
    KASSERT(avl_validate(&tree), "validate");
}

static void
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
    ktest_begin("delete_complex_1");
    struct avl_root tree;
    avl_init(&tree, test_node_compare);

    struct test_node *deleted = create_test_node(10);

    avl_insert(&tree, &deleted->tree_entry);
    int val1[] = {10};
    KASSERT(pre_order_assert(&tree, val1, 1), "delete_complex_1_1");

    avl_remove(&tree, &deleted->tree_entry);

    KASSERT(pre_order_assert(&tree, val1, 0), "delete_complex_1_2");
    KASSERT(avl_validate(&tree), "validate");
}

static void
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
    ktest_begin("delete_complex_2");
    struct avl_root tree;
    avl_init(&tree, test_node_compare);

    struct test_node *deleted = create_test_node(20);

    avl_insert(&tree, &deleted->tree_entry);
    avl_insert(&tree, &create_test_node(10)->tree_entry);
    avl_insert(&tree, &create_test_node(30)->tree_entry);
    avl_insert(&tree, &create_test_node(25)->tree_entry);
    avl_insert(&tree, &create_test_node(35)->tree_entry);
    int val1[] = {20, 10, 30, 25, 35};
    KASSERT(pre_order_assert(&tree, val1, 5), "delete_complex_2_1");

    avl_remove(&tree, &deleted->tree_entry);

    int val2[] = {25, 10, 30, 35};
    KASSERT(pre_order_assert(&tree, val2, 4), "delete_complex_2_2");
    KASSERT(avl_validate(&tree), "validate");
}

static void
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
    ktest_begin("delete_complex_3");
    struct avl_root tree;
    avl_init(&tree, test_node_compare);

    struct test_node *deleted = create_test_node(10);

    avl_insert(&tree, &create_test_node(20)->tree_entry);
    avl_insert(&tree, &deleted->tree_entry);
    avl_insert(&tree, &create_test_node(30)->tree_entry);
    avl_insert(&tree, &create_test_node(5)->tree_entry);
    avl_insert(&tree, &create_test_node(15)->tree_entry);
    avl_insert(&tree, &create_test_node(25)->tree_entry);
    int val1[] = {20, 10, 5, 15, 30, 25};
    KASSERT(pre_order_assert(&tree, val1, 6), "delete_complex_3_1");

    avl_remove(&tree, &deleted->tree_entry);

    int val2[] = {20, 15, 5, 30, 25};
    KASSERT(pre_order_assert(&tree, val2, 5), "delete_complex_3_2");
    KASSERT(avl_validate(&tree), "validate");
}

static void
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
    ktest_begin("delete_complex_4");
    struct avl_root tree;
    avl_init(&tree, test_node_compare);

    struct test_node *delete5 = create_test_node(5);
    struct test_node *delete10 = create_test_node(10);
    struct test_node *delete15 = create_test_node(15);
    struct test_node *delete25 = create_test_node(25);
    struct test_node *delete30 = create_test_node(30);


    avl_insert(&tree, &create_test_node(20)->tree_entry);
    avl_insert(&tree, &delete10->tree_entry);
    avl_insert(&tree, &delete30->tree_entry);
    avl_insert(&tree, &delete5->tree_entry);
    avl_insert(&tree, &delete15->tree_entry);
    avl_insert(&tree, &delete25->tree_entry);

    int val1[] = {20, 10, 5, 15, 30, 25};
    KASSERT(pre_order_assert(&tree, val1, 6), "delete_complex_4_1");

    avl_remove(&tree, &delete5->tree_entry);
    avl_remove(&tree, &delete15->tree_entry);
    avl_remove(&tree, &delete25->tree_entry);
    avl_remove(&tree, &delete10->tree_entry);
    avl_remove(&tree, &delete30->tree_entry);

    int val2[] = {20};
    KASSERT(pre_order_assert(&tree, val2, 1), "delete_complex_4_2");
    KASSERT(avl_validate(&tree), "validate");
}

static void
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
    ktest_begin("delete_complex_single_rotation");
    struct avl_root tree;
    avl_init(&tree, test_node_compare);

    struct test_node *deleted = create_test_node(50);

    avl_insert(&tree, &create_test_node(20)->tree_entry);
    avl_insert(&tree, &create_test_node(10)->tree_entry);
    avl_insert(&tree, &create_test_node(30)->tree_entry);
    avl_insert(&tree, &create_test_node(5)->tree_entry);
    avl_insert(&tree, &create_test_node(15)->tree_entry);
    avl_insert(&tree, &create_test_node(25)->tree_entry);
    avl_insert(&tree, &create_test_node(40)->tree_entry);
    avl_insert(&tree, &create_test_node(12)->tree_entry);
    avl_insert(&tree, &create_test_node(22)->tree_entry);
    avl_insert(&tree, &create_test_node(35)->tree_entry);
    avl_insert(&tree, &deleted->tree_entry);
    avl_insert(&tree, &create_test_node(31)->tree_entry);
    int val1[] = {20, 10, 5, 15, 12, 30, 25, 22, 40, 35, 31, 50};
    KASSERT(pre_order_assert(&tree, val1, 12), "delete_complex_single_rotation_1");

    avl_remove(&tree, &deleted->tree_entry);

    int val2[] = {20, 10, 5, 15, 12, 30, 25, 22, 35, 31, 40};
    KASSERT(pre_order_assert(&tree, val2, 11), "delete_complex_single_rotation_2");
    KASSERT(avl_validate(&tree), "validate");
}

static void
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
    ktest_begin("delete_complex_double_rotation");
    struct avl_root tree;
    avl_init(&tree, test_node_compare);

    struct test_node *deleted = create_test_node(22);

    avl_insert(&tree, &create_test_node(20)->tree_entry);
    avl_insert(&tree, &create_test_node(10)->tree_entry);
    avl_insert(&tree, &create_test_node(30)->tree_entry);
    avl_insert(&tree, &create_test_node(5)->tree_entry);
    avl_insert(&tree, &create_test_node(15)->tree_entry);
    avl_insert(&tree, &create_test_node(25)->tree_entry);
    avl_insert(&tree, &create_test_node(40)->tree_entry);
    avl_insert(&tree, &create_test_node(12)->tree_entry);
    avl_insert(&tree, &deleted->tree_entry);
    avl_insert(&tree, &create_test_node(35)->tree_entry);
    avl_insert(&tree, &create_test_node(50)->tree_entry);
    avl_insert(&tree, &create_test_node(31)->tree_entry);
    int val1[] = {20, 10, 5, 15, 12, 30, 25, 22, 40, 35, 31, 50};
    KASSERT(pre_order_assert(&tree, val1, 12), "delete_complex_double_rotation_1");

    avl_remove(&tree, &deleted->tree_entry);

    int val2[] = {20, 10, 5, 15, 12, 35, 30, 25, 31, 40, 50};
    KASSERT(pre_order_assert(&tree, val2, 11), "delete_complex_double_rotation_2");
    KASSERT(avl_validate(&tree), "validate");
}

static void
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
    ktest_begin("delete_complex_multiple_rotation");
    struct avl_root tree;
    avl_init(&tree, test_node_compare);

    struct test_node *deleted = create_test_node(5);

    avl_insert(&tree, &create_test_node(20)->tree_entry);
    avl_insert(&tree, &create_test_node(10)->tree_entry);
    avl_insert(&tree, &create_test_node(30)->tree_entry);
    avl_insert(&tree, &deleted->tree_entry);
    avl_insert(&tree, &create_test_node(15)->tree_entry);
    avl_insert(&tree, &create_test_node(25)->tree_entry);
    avl_insert(&tree, &create_test_node(40)->tree_entry);
    avl_insert(&tree, &create_test_node(12)->tree_entry);
    avl_insert(&tree, &create_test_node(22)->tree_entry);
    avl_insert(&tree, &create_test_node(35)->tree_entry);
    avl_insert(&tree, &create_test_node(50)->tree_entry);
    avl_insert(&tree, &create_test_node(31)->tree_entry);
    int val1[] = {20, 10, 5, 15, 12, 30, 25, 22, 40, 35, 31, 50};
    KASSERT(pre_order_assert(&tree, val1, 12), "delete_complex_multiple_rotation_1");

    avl_remove(&tree, &deleted->tree_entry);

    int val2[] = {30, 20, 12, 10, 15, 25, 22, 40, 35, 31, 50};
    KASSERT(pre_order_assert(&tree, val2, 11), "delete_complex_multiple_rotation_2");
    KASSERT(avl_validate(&tree), "validate");
}

static void
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
    ktest_begin("delete_DNE");
    struct avl_root tree;
    avl_init(&tree, test_node_compare);

    struct test_node *delete100 = create_test_node(100);
    struct test_node *delete24 = create_test_node(24);

    avl_insert(&tree, &create_test_node(20)->tree_entry);
    avl_insert(&tree, &create_test_node(10)->tree_entry);
    avl_insert(&tree, &create_test_node(30)->tree_entry);
    avl_insert(&tree, &create_test_node(5)->tree_entry);
    avl_insert(&tree, &create_test_node(15)->tree_entry);
    avl_insert(&tree, &create_test_node(25)->tree_entry);

    int val1[] = {20, 10, 5, 15, 30, 25};
    KASSERT(pre_order_assert(&tree, val1, 6), "delete_DNE_1");

    avl_remove(&tree, &delete24->tree_entry);
    avl_remove(&tree, &delete100->tree_entry);
    KASSERT(pre_order_assert(&tree, val1, 6), "delete_DNE_2");
    KASSERT(avl_validate(&tree), "validate");
}

static void
test_apocalypse(void)
{
    struct avl_root tree;

    ktest_begin("test_apocalypse");

    ksrand(1337523847);

    avl_init(&tree, test_node_compare);

    // insert test
    for (int i = 0; i < AVL_BRUTE_TEST_NODE; i++) {
        avl_alloc_nodes[i].val = krand();
        while (avl_search(&tree, &avl_alloc_nodes[i].tree_entry) != NULL) {
            avl_alloc_nodes[i].val += krand() % 32765;
        }
        avl_insert(&tree, &avl_alloc_nodes[i].tree_entry);
    }

    // integrity test
    KASSERT(avl_validate(&tree), "validate");
    KASSERT(avl_size(&tree) == AVL_BRUTE_TEST_NODE, "test_apo_sz_1");

    // smaller and bigger test
    struct avl_node *entry = avl_first(&tree);
    uint32 size = 0;
    int32 prev = -1;
    int32 cur = OBTAIN_STRUCT_ADDR(entry, struct test_node, tree_entry)->val;
    while (entry != NULL) {
        if (cur < prev) {
            KASSERT(0, "test_apo_order_1");
            break;
        }
        size++;
        entry = avl_next(&tree, entry);
        prev = cur;
        if (entry != NULL) {
            cur = OBTAIN_STRUCT_ADDR(entry, struct test_node, tree_entry)->val;
        }
    }

    KASSERT(size == AVL_BRUTE_TEST_NODE, "test_apo_1");

    // larger test
    entry = avl_last(&tree);
    size = 0;
    cur = OBTAIN_STRUCT_ADDR(entry, struct test_node, tree_entry)->val;
    prev = cur;
    while (entry != NULL) {
        if (cur > prev) {
            KASSERT(0, "test_apo_order_1");
            break;
        }
        size++;
        entry = avl_prev(&tree, entry);
        prev = cur;
        if (entry != NULL) {
            cur = OBTAIN_STRUCT_ADDR(entry, struct test_node, tree_entry)->val;
        }
    }

    KASSERT(size == AVL_BRUTE_TEST_NODE, "test_apo_2");


    // delete and search test
    for (int i = 0; i < AVL_BRUTE_TEST_NODE; i++) {
        KASSERT((avl_search(&tree, &avl_alloc_nodes[i].tree_entry) != NULL), "test_apo_search_1");
        avl_remove(&tree, &avl_alloc_nodes[i].tree_entry);
        KASSERT((avl_search(&tree, &avl_alloc_nodes[i].tree_entry) == NULL), "test_apo_search_2");
        KASSERT(avl_validate(&tree), "test_apo_validate_2");
    }

    KASSERT((avl_size(&tree) == 0), "test_apo_sz_2");
}

static void
avl_tree_test(ATTR_UNUSED void *unused)
{
    insert_simple_l();
    insert_simple_r();
    insert_simple_ll();
    insert_simple_rr();

    // complex ones
    insert_complex_1();
    insert_complex_2();
    insert_complex_3();
    insert_complex_4();

    // insert duplicate
    insert_duplicate();

    // simple tests
    delete_simple_l();
    delete_simple_r();
    delete_simple_ll();
    delete_simple_rr();

    // complex tests
    delete_complex_1();
    delete_complex_2();
    delete_complex_3();
    delete_complex_4();
    delete_complex_single_rotation();
    delete_complex_double_rotation();
    delete_complex_multiple_rotation();
    delete_DNE();

    /* clear all memory */
    test_apocalypse();
}

KTEST_DECL(avl_tree, KTEST_SUBSYS_AVL, avl_tree_test, NULL);

