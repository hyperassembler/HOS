#include <test/ktest.h>
#include <common/cdef.h>
#include <common/list.h>
#include <common/libkern.h>

#define ARR_SZ(arr) (sizeof(arr) / sizeof((arr)[0]))

struct test_list_node {
    struct list_entry lnode;
    int val;
};

static void
validate_list(struct list_entry *list)
{
    if (list_empty(list)) {
        return;
    }

    KASSERT(!(list->prev == NULL || list->next == NULL), "list head ptr corrupted");

    struct list_entry *it;
    LIST_FOREACH(list, it) {
        KASSERT(it->next->prev == it && it->prev->next == it, "forward list entry ptr corrupted");
    }

    LIST_FOREACH_REVERSE(list, it) {
        KASSERT(it->next->prev == it && it->prev->next == it, "backward list entry ptr corrupted");
    }
}

//static void
//print_list(struct list_entry *list)
//{
//    struct list_entry *e;
//
//    LIST_FOREACH(list, e) {
//        struct test_list_node *enode = OBTAIN_STRUCT_ADDR(e, struct test_list_node, lnode);
//        kprintf("%d->", enode->val);
//    }
//    kprintf("[END]\n");
//}

static void
check_list_elements(struct list_entry *list, const int *val, int size)
{
    struct list_entry *node;
    int i = 0;

    LIST_FOREACH(list, node) {
        struct test_list_node *enode = OBTAIN_STRUCT_ADDR(node, struct test_list_node, lnode);
        KASSERT(enode->val == val[i], "check list element failed at idx %d: %d != %d", i, enode->val, val[i]);
        i++;
    }

    KASSERT(i == size, "list size != expected size");
}

static void
assert_list(struct list_entry *list, const int *val, int size)
{
    validate_list(list);
    check_list_elements(list, val, size);
}

static void
insert_test_front(void)
{
    ktest_begin("insert_test_front");

    struct list_entry list;
    struct test_list_node n0 = {.val = 0};
    struct test_list_node n1 = {.val = 1};
    struct test_list_node n2 = {.val = 2};
    struct test_list_node n3 = {.val = 3};

    list_init(&list);
    list_insert(&list, &n0.lnode);
    list_insert(&list, &n1.lnode);
    list_insert(&list, &n2.lnode);
    list_insert(&list, &n3.lnode);

    int val[] = {3, 2, 1, 0};
    assert_list(&list, val, ARR_SZ(val));
}

static void
insert_test_before(void)
{
    ktest_begin("insert_test_before");

    struct list_entry list;
    struct test_list_node n0 = {.val = 0};
    struct test_list_node n1 = {.val = 1};
    struct test_list_node n2 = {.val = 2};
    struct test_list_node n3 = {.val = 3};

    list_init(&list);
    list_insert_before(&list, &n0.lnode);
    list_insert_before(&list, &n1.lnode);
    list_insert_before(&list, &n2.lnode);
    list_insert_before(&list, &n3.lnode);

    int val[] = {0, 1, 2, 3};
    assert_list(&list, val, ARR_SZ(val));
}

static void
insert_test_middle(void)
{
    ktest_begin("insert_test_middle");

    struct list_entry list;
    struct test_list_node n0 = {.val = 0};
    struct test_list_node n1 = {.val = 1};
    struct test_list_node n2 = {.val = 2};
    struct test_list_node n4 = {.val = 4};
    struct test_list_node n5 = {.val = 5};
    struct test_list_node n6 = {.val = 6};

    list_init(&list);
    list_insert(&list, &n0.lnode);
    list_insert(&list, &n1.lnode);
    list_insert(&list, &n2.lnode);

    list_insert(&n1.lnode, &n4.lnode);
    list_insert(&n1.lnode, &n5.lnode);
    list_insert_before(&n4.lnode, &n6.lnode);

    int val[] = {2, 1, 5, 6, 4, 0};
    assert_list(&list, val, ARR_SZ(val));
}

static void
insert_test_back(void)
{
    ktest_begin("insert_test_back");

    struct list_entry list;
    struct test_list_node n0 = {.val = 0};
    struct test_list_node n1 = {.val = 1};
    struct test_list_node n2 = {.val = 2};
    struct test_list_node n3 = {.val = 3};

    list_init(&list);
    list_insert(&list, &n0.lnode);
    list_insert(&n0.lnode, &n1.lnode);
    list_insert(&n1.lnode, &n2.lnode);
    list_insert(&n2.lnode, &n3.lnode);

    int val[] = {0, 1, 2, 3};
    assert_list(&list, val, ARR_SZ(val));
}

static void
remove_test_front(void)
{
    ktest_begin("remove_test_front");

    struct list_entry list;
    struct test_list_node n0 = {.val = 0};
    struct test_list_node n1 = {.val = 1};
    struct test_list_node n2 = {.val = 2};
    struct test_list_node n3 = {.val = 3};

    list_init(&list);
    list_insert(&list, &n0.lnode);
    list_insert(&list, &n1.lnode);
    list_insert(&list, &n2.lnode);
    list_insert(&list, &n3.lnode);

    list_remove_after(&list);
    list_remove_after(&list);

    int val[] = {1, 0};
    assert_list(&list, val, ARR_SZ(val));
}

static void
remove_test_middle(void)
{
    ktest_begin("remove_test_middle");

    struct list_entry list;
    struct test_list_node n0 = {.val = 0};
    struct test_list_node n1 = {.val = 1};
    struct test_list_node n2 = {.val = 2};
    struct test_list_node n3 = {.val = 3};

    list_init(&list);
    list_insert(&list, &n0.lnode);
    list_insert(&list, &n1.lnode);
    list_insert(&list, &n2.lnode);
    list_insert(&list, &n3.lnode);

    list_remove(&n1.lnode);
    list_remove(&n2.lnode);

    int val[] = {3, 0};
    assert_list(&list, val, ARR_SZ(val));
}

static void
remove_test_end(void)
{
    ktest_begin("remove_test_middle");

    struct list_entry list;
    struct test_list_node n0 = {.val = 0};
    struct test_list_node n1 = {.val = 1};
    struct test_list_node n2 = {.val = 2};
    struct test_list_node n3 = {.val = 3};

    list_init(&list);
    list_insert(&list, &n0.lnode);
    list_insert(&list, &n1.lnode);
    list_insert(&list, &n2.lnode);
    list_insert(&list, &n3.lnode);

    list_remove_before(&list);
    list_remove_before(&list);

    int val[] = {3, 2};
    assert_list(&list, val, ARR_SZ(val));
}

static void
remove_test_all(void)
{
    ktest_begin("remove_test_all");

    struct list_entry list;
    struct test_list_node n0 = {.val = 0};
    struct test_list_node n1 = {.val = 1};
    struct test_list_node n2 = {.val = 2};
    struct test_list_node n3 = {.val = 3};

    list_init(&list);
    list_insert(&list, &n0.lnode);
    list_insert(&list, &n1.lnode);
    list_insert(&list, &n2.lnode);
    list_insert(&list, &n3.lnode);

    list_remove_after(&list);
    list_remove_after(&list);
    list_remove_after(&list);
    list_remove_after(&list);

    assert_list(&list, NULL, 0);

    list_insert(&list, &n0.lnode);
    list_insert(&list, &n1.lnode);
    list_insert(&list, &n2.lnode);
    list_insert(&list, &n3.lnode);

    int val[] = {0, 1, 2, 3};
    assert_list(&list, val, ARR_SZ(val));
}

static void
list_test(ATTR_UNUSED void *unused)
{
    insert_test_front();
    insert_test_before();
    insert_test_middle();
    insert_test_back();

    remove_test_front();
    remove_test_middle();
    remove_test_end();
    remove_test_all();
}

KTEST_DECL(list, KTEST_SUBSYS_LIST, list_test, NULL);
