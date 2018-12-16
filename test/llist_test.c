#include "test_main.h"
#include "test_case.h"
#include "lb/dlist.h"
#include "kern/clib.h"
#include <stdio.h>

struct test_list_node
{
    struct llist_node lnode;
    int32 val;
};

static bool
validate_list(struct dlist *list)
{
    bool result = TRUE;
    // list_head_test
    if (list->head != NULL)
    {
        result = result && (list->head->prev == NULL);
    }
    else
    {
        result = result && (list->tail == NULL);
    }

    if (list->tail != NULL)
    {
        result = result && (list->tail->next == NULL);
    }
    else
    {
        result = result && (list->head == NULL);
    }

    return result;
}

static void
print_list(struct dlist *list)
{
#ifdef TDBG
    struct llist_node *node = lb_dlist_first(list);

    while (node != NULL)
    {
        struct test_list_node *enode = OBTAIN_STRUCT_ADDR(node, struct test_list_node, lnode);
        printf("%d->", enode->val);
        node = lb_dlist_next(node);
    }
    printf("[END]\n");
#endif
}

static bool
check_list_elements(struct dlist *list, int val[], int size)
{
    struct llist_node *node = list->head;
    bool ret = TRUE;
    int i = 0;
    while (node != NULL && i < size)
    {
        struct test_list_node *enode = OBTAIN_STRUCT_ADDR(node, struct test_list_node, lnode);
        if (enode->val != val[i])
        {
            ret = FALSE;
            break;
        }
        i++;
        node = lb_dlist_next(node);
    }

    if(ret)
    {
        if (i != size)
        {
            ret = FALSE;
        }
    }

    if(ret)
    {
        node = lb_dlist_last(list);
        while (node != NULL && i >= 0)
        {
            struct test_list_node *enode = OBTAIN_STRUCT_ADDR(node, struct test_list_node, lnode);
            if (enode->val != val[i - 1])
            {
                ret = FALSE;
                break;
            }
            i--;
            node = lb_dlist_prev(node);
        }
    }

    if(ret)
    {
        ret = ret && (i == 0);
    }

#ifdef TDBG
    if (!ret)
    {
        printf("[LLIST ASSERT] Expected: ");
        for (i = 0; i < size; i++)
        {
            printf("%d-", val[i]);
        }
        printf("\n             Got:");
        print_list(list);
        printf("\n");
        fflush(stdout);
    }
#endif
    return ret;
}


static bool
assert_list(struct dlist *list, int val[], int size)
{
    struct llist_node *node = lb_dlist_first(list);
    int i = 0;

    if (!validate_list(list))
    {
        return FALSE;
    }

    return check_list_elements(list, val, size);
}

static void
insert_val(struct dlist *list, int index, int val)
{
    struct test_list_node *a = (struct test_list_node *) talloc(sizeof(struct test_list_node));
    a->val = val;
    lb_llist_insert_by_idx(list, index, &a->lnode);
}

static void
push_back_val(struct dlist *list, int val)
{
    struct test_list_node *a = (struct test_list_node *) talloc(sizeof(struct test_list_node));
    a->val = val;
    lb_llist_push_back(list, &a->lnode);
}

static void
push_front_val(struct dlist *list, int val)
{
    struct test_list_node *a = (struct test_list_node *) talloc(sizeof(struct test_list_node));
    a->val = val;
    lb_llist_push_front(list, &a->lnode);
}


static bool
insert_test_beginning(void)
{
    struct dlist list;
    lb_dlist_init(&list);
    insert_val(&list, 0, 0);
    insert_val(&list, 0, 1);
    insert_val(&list, 0, 2);
    insert_val(&list, 0, 3);

    // 3210==0123
    int val[4] = {3, 2, 1, 0};
    return assert_list(&list, val, 4);
}

static bool
insert_test_middle(void)
{
    struct dlist list;
    lb_dlist_init(&list);

    insert_val(&list, 0, 0);
    insert_val(&list, 0, 1);
    insert_val(&list, 0, 2);

    insert_val(&list, 1, 4);
    insert_val(&list, 1, 5);
    insert_val(&list, 2, 6);

    int val[] = {2, 5, 6, 4, 1, 0};
    return assert_list(&list, val, 6);
}

static bool
insert_test_end(void)
{
    struct dlist list;
    lb_dlist_init(&list);

    insert_val(&list, 0, 0);
    insert_val(&list, 1, 1);
    insert_val(&list, 2, 2);
    insert_val(&list, 3, 3);

    int val[] = {0, 1, 2, 3};
    return assert_list(&list, val, 4);
}

static bool
insert_test_invalid(void)
{
    struct dlist list;
    lb_dlist_init(&list);

    insert_val(&list, 0, 3);
    insert_val(&list, 0, 2);
    insert_val(&list, 0, 1);
    insert_val(&list, 0, 0);

    // large index
    insert_val(&list, 5, 9);
    insert_val(&list, 6, 9);
    insert_val(&list, 999, 9);

    // small index
    insert_val(&list, -1, 8);
    insert_val(&list, -2, 8);
    insert_val(&list, -999, 8);

    /**
     * Since it's kernel library
     * Don't test NULL
     */
    /*
   insert_val(NULL, 1, 4);
   lb_llist_insert_by_ref(NULL, list.head, list.tail);
   lb_llist_insert_by_ref(&list, list.head, NULL);
     */

    int val[] = {0, 1, 2, 3};
    return assert_list(&list, val, 4);
}


static bool
remove_test_beginning(void)
{
    struct dlist list;
    lb_dlist_init(&list);
    insert_val(&list, 0, 0);
    insert_val(&list, 0, 1);
    insert_val(&list, 0, 2);
    insert_val(&list, 0, 3);

    lb_llist_remove_by_idx(&list, 0);
    lb_llist_remove_by_idx(&list, 0);

    // 10==01
    int val[] = {1, 0};
    return assert_list(&list, val, 2);
}

static bool
remove_test_middle(void)
{
    struct dlist list;
    lb_dlist_init(&list);

    insert_val(&list, 0, 0);
    insert_val(&list, 0, 1);
    insert_val(&list, 0, 2);

    insert_val(&list, 0, 3);
    insert_val(&list, 0, 4);
    insert_val(&list, 0, 5);

    lb_llist_remove_by_idx(&list, 1);
    lb_llist_remove_by_idx(&list, 2);

    // 5310=====0135
    int val[] = {5, 3, 1, 0};
    return assert_list(&list, val, 4);
}

static bool
remove_test_end(void)
{
    struct dlist list;
    lb_dlist_init(&list);

    insert_val(&list, 0, 0);
    insert_val(&list, 1, 1);
    insert_val(&list, 2, 2);
    insert_val(&list, 3, 3);

    lb_llist_remove_by_idx(&list, 3);
    lb_llist_remove_by_idx(&list, 2);

    int val[] = {0, 1};
    return assert_list(&list, val, 2);
}

static bool
remove_test_all(void)
{
    bool result = TRUE;
    struct dlist list;
    lb_dlist_init(&list);

    insert_val(&list, 0, 0);
    insert_val(&list, 1, 1);
    insert_val(&list, 2, 2);
    insert_val(&list, 3, 3);

    lb_llist_remove_by_idx(&list, 0);
    lb_llist_remove_by_idx(&list, 0);
    lb_llist_remove_by_idx(&list, 0);
    lb_llist_remove_by_idx(&list, 0);

    result = result && assert_list(&list, NULL, 0);

    insert_val(&list, 0, 0);
    insert_val(&list, 1, 1);
    insert_val(&list, 2, 2);
    insert_val(&list, 3, 3);

    lb_llist_remove_by_idx(&list, 3);
    lb_llist_remove_by_idx(&list, 2);
    lb_llist_remove_by_idx(&list, 1);
    lb_llist_remove_by_idx(&list, 0);

    result = result && assert_list(&list, NULL, 0);

    insert_val(&list, 0, 0);
    insert_val(&list, 1, 1);
    insert_val(&list, 2, 2);
    insert_val(&list, 3, 3);

    lb_llist_remove_by_idx(&list, 1);
    lb_llist_remove_by_idx(&list, 1);
    lb_llist_remove_by_idx(&list, 1);
    lb_llist_remove_by_idx(&list, 0);

    result = result && assert_list(&list, NULL, 0);

    return result;
}

static bool
remove_test_invalid(void)
{
    struct dlist list;
    lb_dlist_init(&list);

    insert_val(&list, 0, 3);
    insert_val(&list, 0, 2);
    insert_val(&list, 0, 1);
    insert_val(&list, 0, 0);

    // large index
    lb_llist_remove_by_idx(&list, 5);
    lb_llist_remove_by_idx(&list, 6);
    lb_llist_remove_by_idx(&list, 999);

    // small index
    lb_llist_remove_by_idx(&list, -1);
    lb_llist_remove_by_idx(&list, -2);
    lb_llist_remove_by_idx(&list, -999);

    /**
     * Since it's kernel library
     * Don't test NULL
     */
    /*
    lb_llist_remove_by_idx(NULL, 1);
    lb_llist_remove_by_ref(NULL, list.head);
    lb_llist_remove_by_ref(&list, NULL); */

    // 0123=====3210
    int val[] = {0, 1, 2, 3};
    return assert_list(&list, val, 4);
}

static bool
size_test(void)
{
    bool result = TRUE;
    struct dlist list;
    lb_dlist_init(&list);
    struct dlist list2;
    lb_dlist_init(&list2);

    insert_val(&list, 0, 0);
    insert_val(&list, 1, 1);
    insert_val(&list, 2, 2);
    insert_val(&list, 3, 3);

    /**
    * Since it's kernel library
    * Don't test NULL
    */
    /*
     * lb_llist_size(NULL) == -1
     */
    result = result && (lb_llist_size(&list) == 4 && lb_llist_size(&list2) == 0);

    lb_llist_remove_by_idx(&list, 0);
    result = result && (lb_llist_size(&list) == 3);
    insert_val(&list, 0, 0);

    int val[] = {0, 1, 2, 3};
    result = result && assert_list(&list, val, 4);
    return result;
}

static bool
push_pop_front_test(void)
{
    struct llist_node *node;
    bool result = TRUE;
    struct dlist list;
    lb_dlist_init(&list);

    push_front_val(&list, 1);
    push_front_val(&list, 2);
    push_front_val(&list, 3);
    push_front_val(&list, 4);

    //4321==1234
    int val1[] = {4, 3, 2, 1};
    result = result && assert_list(&list, val1, 4);

    node = lb_llist_pop_front(&list);
    //321==123
    int val2[] = {3, 2, 1};
    result = result && assert_list(&list, val2, 3) && OBTAIN_STRUCT_ADDR(node, struct test_list_node, lnode)->val == 4;

    lb_llist_pop_front(&list);
    lb_llist_pop_front(&list);
    node = lb_llist_pop_front(&list);

    result = result && assert_list(&list, NULL, 0) && OBTAIN_STRUCT_ADDR(node, struct test_list_node, lnode)->val == 1;
    return result;
}

static bool
push_pop_back_test(void)
{
    bool result = TRUE;
    struct dlist list;
    lb_dlist_init(&list);
    struct llist_node *node;

    push_back_val(&list, 1);
    push_back_val(&list, 2);
    push_back_val(&list, 3);
    push_back_val(&list, 4);

    //1234==4321
    int val1[] = {1, 2, 3, 4};
    result = result && assert_list(&list, val1, 4);

    node = lb_llist_pop_back(&list);
    //123==321
    int val2[] = {1, 2, 3};
    result = result && assert_list(&list, val2, 3) && OBTAIN_STRUCT_ADDR(node, struct test_list_node, lnode)->val == 4;

    lb_llist_pop_back(&list);
    node = lb_llist_pop_back(&list);
    lb_llist_pop_back(&list);

    result = result && assert_list(&list, NULL, 0) && OBTAIN_STRUCT_ADDR(node, struct test_list_node, lnode)->val == 2;
    return result;
}

void
linked_list_test(void)
{
    test_begin("Linked list test");
    run_case("insert_test_beginning", insert_test_beginning());
    run_case("insert_test_middle", insert_test_middle());
    run_case("insert_test_end", insert_test_end());
    run_case("insert_test_invalid", insert_test_invalid());

    run_case("remove_test_beginning", remove_test_beginning());
    run_case("remove_test_middle", remove_test_middle());
    run_case("remove_test_end", remove_test_end());
    run_case("remove_test_invalid", remove_test_invalid());

    run_case("size_test", size_test());

    run_case("remove_test_all", remove_test_all());

    run_case("push_pop_front_test", push_pop_front_test());
    run_case("push_pop_back_test", push_pop_back_test());

    test_end();
}

