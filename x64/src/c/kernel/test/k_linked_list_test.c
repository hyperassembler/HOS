#include "k_test_driver.h"
#include "k_type.h"
#include "linked_list.h"
#include "k_lib_test.h"
#include "std_lib.h"

typedef struct
{
    linked_list_entry_t lnode;
    int val;
} my_list_node;

static bool validate_list(linked_list_t *list)
{
    bool result = true;
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


static bool assert_list(linked_list_t *list, int val[], int size)
{
    linked_list_entry_t *node = linked_list_first(list);
    int i = 0;

    if (!validate_list(list))
        return false;

    while (node != NULL && i < size)
    {
        my_list_node *enode = OBTAIN_STRUCT_ADDR(node, lnode, my_list_node);
        if (enode->val != val[i])
        {
            return false;
        }
        i++;
        node = linked_list_next(node);
    }

    if (i != size)
    {
        return false;
    }

    node = linked_list_last(list);
    while (node != NULL && i >= 0)
    {
        my_list_node *enode = OBTAIN_STRUCT_ADDR(node, lnode, my_list_node);
        if (enode->val != val[i - 1])
        {
            return false;
        }
        i--;
        node = linked_list_prev(node);
    }

    return i == 0;
}

//void print_validate(linked_list_t *list)
//{
//    my_list_node* node = (my_list_node*) linked_list_first(list);
//    while(node != NULL)
//    {
//        hal_printf("%d", node->val);
//        node = (my_list_node*) linked_list_next((linked_list_node_t *) node);
//    }
//
//    hal_printf("======");
//    node = (my_list_node*) linked_list_last(list);
//    while(node != NULL)
//    {
//        hal_printf("%d", node->val);
//        node = (my_list_node*) linked_list_prev((linked_list_node_t *) node);
//    }
//
//    validate_list(list);
//    hal_printf("\n");
//    return;
//}

static void insert_val(linked_list_t *list, int index, int val)
{
    my_list_node *a = (my_list_node *) talloc(sizeof(my_list_node));
    a->val = val;
    linked_list_insert_idx(list, index, &a->lnode);
}

static void push_back_val(linked_list_t *list, int val)
{
    my_list_node *a = (my_list_node *) talloc(sizeof(my_list_node));
    a->val = val;
    linked_list_push_back(list, &a->lnode);
}

static void push_front_val(linked_list_t *list, int val)
{
    my_list_node *a = (my_list_node *) talloc(sizeof(my_list_node));
    a->val = val;
    linked_list_push_front(list, &a->lnode);
}


static bool insert_test_beginning()
{
    linked_list_t list;
    linked_list_init(&list);
    insert_val(&list, 0, 0);
    insert_val(&list, 0, 1);
    insert_val(&list, 0, 2);
    insert_val(&list, 0, 3);

    // 3210==0123
    int val[4] = {3, 2, 1, 0};
    return assert_list(&list, val, 4);
}

static bool insert_test_middle()
{
    linked_list_t list;
    linked_list_init(&list);

    insert_val(&list, 0, 0);
    insert_val(&list, 0, 1);
    insert_val(&list, 0, 2);

    insert_val(&list, 1, 4);
    insert_val(&list, 1, 5);
    insert_val(&list, 2, 6);

    int val[] = {2, 5, 6, 4, 1, 0};
    return assert_list(&list, val, 6);
}

static bool insert_test_end()
{
    linked_list_t list;
    linked_list_init(&list);

    insert_val(&list, 0, 0);
    insert_val(&list, 1, 1);
    insert_val(&list, 2, 2);
    insert_val(&list, 3, 3);

    int val[] = {0, 1, 2, 3};
    return assert_list(&list, val, 4);
}

static bool insert_test_invalid()
{
    linked_list_t list;
    linked_list_init(&list);

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

    // NULL
    insert_val(NULL, 1, 4);
    linked_list_insert_ref(NULL, list.head, list.tail);
    linked_list_insert_ref(&list, list.head, NULL);

    int val[] = {0, 1, 2, 3};
    return assert_list(&list, val, 4);
}


static bool remove_test_beginning()
{
    linked_list_t list;
    linked_list_init(&list);
    insert_val(&list, 0, 0);
    insert_val(&list, 0, 1);
    insert_val(&list, 0, 2);
    insert_val(&list, 0, 3);

    linked_list_remove_idx(&list, 0);
    linked_list_remove_idx(&list, 0);

    // 10==01
    int val[] = {1, 0};
    return assert_list(&list, val, 2);
}

static bool remove_test_middle()
{
    linked_list_t list;
    linked_list_init(&list);

    insert_val(&list, 0, 0);
    insert_val(&list, 0, 1);
    insert_val(&list, 0, 2);

    insert_val(&list, 0, 3);
    insert_val(&list, 0, 4);
    insert_val(&list, 0, 5);

    linked_list_remove_idx(&list, 1);
    linked_list_remove_idx(&list, 2);

    // 5310=====0135
    int val[] = {5, 3, 1, 0};
    return assert_list(&list, val, 4);
}

static bool remove_test_end()
{
    linked_list_t list;
    linked_list_init(&list);

    insert_val(&list, 0, 0);
    insert_val(&list, 1, 1);
    insert_val(&list, 2, 2);
    insert_val(&list, 3, 3);

    linked_list_remove_idx(&list, 3);
    linked_list_remove_idx(&list, 2);

    int val[] = {0, 1};
    return assert_list(&list, val, 2);
}

static bool remove_test_all()
{
    bool result = true;
    linked_list_t list;
    linked_list_init(&list);

    insert_val(&list, 0, 0);
    insert_val(&list, 1, 1);
    insert_val(&list, 2, 2);
    insert_val(&list, 3, 3);

    linked_list_remove_idx(&list, 0);
    linked_list_remove_idx(&list, 0);
    linked_list_remove_idx(&list, 0);
    linked_list_remove_idx(&list, 0);

    result = result && assert_list(&list, NULL, 0);

    insert_val(&list, 0, 0);
    insert_val(&list, 1, 1);
    insert_val(&list, 2, 2);
    insert_val(&list, 3, 3);

    linked_list_remove_idx(&list, 3);
    linked_list_remove_idx(&list, 2);
    linked_list_remove_idx(&list, 1);
    linked_list_remove_idx(&list, 0);

    result = result && assert_list(&list, NULL, 0);

    insert_val(&list, 0, 0);
    insert_val(&list, 1, 1);
    insert_val(&list, 2, 2);
    insert_val(&list, 3, 3);

    linked_list_remove_idx(&list, 1);
    linked_list_remove_idx(&list, 1);
    linked_list_remove_idx(&list, 1);
    linked_list_remove_idx(&list, 0);

    result = result && assert_list(&list, NULL, 0);

    return result;
}

static bool remove_test_invalid()
{
    linked_list_t list;
    linked_list_init(&list);

    insert_val(&list, 0, 3);
    insert_val(&list, 0, 2);
    insert_val(&list, 0, 1);
    insert_val(&list, 0, 0);

    // large index
    linked_list_remove_idx(&list, 5);
    linked_list_remove_idx(&list, 6);
    linked_list_remove_idx(&list, 999);

    // small index
    linked_list_remove_idx(&list, -1);
    linked_list_remove_idx(&list, -2);
    linked_list_remove_idx(&list, -999);

    // NULL
    linked_list_remove_idx(NULL, 1);
    linked_list_remove_ref(NULL, list.head);
    linked_list_remove_ref(&list, NULL);

    // 0123=====3210
    int val[] = {0, 1, 2, 3};
    return assert_list(&list, val, 4);
}

static bool size_test()
{
    bool result = true;
    linked_list_t list;
    linked_list_init(&list);
    linked_list_t list2;
    linked_list_init(&list2);

    insert_val(&list, 0, 0);
    insert_val(&list, 1, 1);
    insert_val(&list, 2, 2);
    insert_val(&list, 3, 3);

    result = result && (linked_list_size(&list) == 4 && linked_list_size(&list2) == 0 && linked_list_size(NULL) == -1);
    int val[] = {0, 1, 2, 3};
    result = result && assert_list(&list, val, 4);
    return result;
}

static bool push_pop_front_test()
{
    bool result = true;
    linked_list_t list;
    linked_list_init(&list);

    push_front_val(&list, 1);
    push_front_val(&list, 2);
    push_front_val(&list, 3);
    push_front_val(&list, 4);

    //4321==1234
    int val1[] = {4, 3, 2, 1};
    result = result && assert_list(&list, val1, 4);

    linked_list_pop_front(&list);
    //321==123
    int val2[] = {3, 2, 1};
    result = result && assert_list(&list, val2, 3);

    linked_list_pop_front(&list);
    linked_list_pop_front(&list);
    linked_list_pop_front(&list);

    result = result && assert_list(&list, NULL, 0);
    return result;
}

static bool push_pop_back_test()
{
    bool result = true;
    linked_list_t list;
    linked_list_init(&list);

    push_back_val(&list, 1);
    push_back_val(&list, 2);
    push_back_val(&list, 3);
    push_back_val(&list, 4);

    //1234==4321
    int val1[] = {1, 2, 3, 4};
    result = result && assert_list(&list, val1, 4);

    linked_list_pop_back(&list);
    //123==321
    int val2[] = {1, 2, 3};
    result = result && assert_list(&list, val2, 3);

    linked_list_pop_back(&list);
    linked_list_pop_back(&list);
    linked_list_pop_back(&list);

    result = result && assert_list(&list, NULL, 0);
    return result;
}

static bool equals(linked_list_entry_t *a, linked_list_entry_t *b)
{
    return (int64_t) a == OBTAIN_STRUCT_ADDR(b, lnode, my_list_node)->val;
}

static bool search_test()
{
    bool result = true;
    linked_list_t list;
    linked_list_init(&list);

    push_back_val(&list, 1);
    push_back_val(&list, 2);
    push_back_val(&list, 3);
    push_back_val(&list, 4);

    int val1[] = {1, 2, 3, 4};
    result = result && assert_list(&list, val1, 4);

    result = result && (linked_list_search(&list, (linked_list_entry_t *) 4, equals) == 3);
    result = result && (linked_list_search(&list, (linked_list_entry_t *) 3, equals) == 2);
    result = result && (linked_list_search(&list, (linked_list_entry_t *) 2, equals) == 1);
    result = result && (linked_list_search(&list, (linked_list_entry_t *) 1, equals) == 0);

    result = result && (linked_list_search(&list, NULL, equals) == -1);
    result = result && (linked_list_search(NULL, (linked_list_entry_t *) 1, equals) == -1);

    linked_list_entry_t *node = linked_list_get(&list, 1);
    result = result && (linked_list_search(&list, node, NULL) == 1);


    result = result && assert_list(&list, val1, 4);

    return result;
}


void SAPI linked_list_test(void)
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

    run_case("search_test", search_test());
    test_end();
}

