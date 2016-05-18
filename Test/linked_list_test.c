#include <stdlib.h>
#include <printf.h>
#include "linked_list.h"

typedef struct
{
    linked_list_node_t lnode;
    int val;
} my_list;

void validate_list(linked_list_t* list)
{
    int good = 0;
    if(list->head == NULL)
        good += list->tail == NULL;
    if(list->tail == NULL)
        good += list->head == NULL;
    if(list->head != NULL)
        good += list->head->prev == NULL;
    if(list->tail != NULL)
        good += list->tail->next == NULL;
    printf(good == 2 ? "   good" : "  bad");
}

void print_validate(linked_list_t *list)
{
    my_list* node = (my_list*) linked_list_first(list);
    while(node != NULL)
    {
        printf("%d", node->val);
        node = (my_list*) linked_list_next((linked_list_node_t *) node);
    }

    printf("======");
    node = (my_list*) linked_list_last(list);
    while(node != NULL)
    {
        printf("%d", node->val);
        node = (my_list*) linked_list_prev((linked_list_node_t *) node);
    }

    validate_list(list);
    printf("\n");
    return;
}

void insert_val(linked_list_t* list, int index, int val)
{
    my_list *a = (my_list*)malloc(sizeof(my_list));
    a->val = val;
    linked_list_insert_idx(list, index, &a->lnode);
}

void push_back_val(linked_list_t* list, int val)
{
    my_list *a = (my_list*)malloc(sizeof(my_list));
    a->val = val;
    linked_list_push_back(list, &a->lnode);
}

void push_front_val(linked_list_t* list, int val)
{
    my_list *a = (my_list*)malloc(sizeof(my_list));
    a->val = val;
    linked_list_push_front(list, &a->lnode);
}


void insert_test_beginning()
{
    linked_list_t list;
    linked_list_init(&list);
    insert_val(&list, 0, 0);
    insert_val(&list, 0, 1);
    insert_val(&list, 0, 2);
    insert_val(&list, 0, 3);

    // 3210==0123
    print_validate(&list);
}

void insert_test_middle()
{
    linked_list_t list;
    linked_list_init(&list);

    insert_val(&list, 0, 0);
    insert_val(&list, 0, 1);
    insert_val(&list, 0, 2);

    insert_val(&list, 1, 4);
    insert_val(&list, 1, 5);
    insert_val(&list, 2, 6);

    // 256410=====014652
    print_validate(&list);
}

void insert_test_end()
{
    linked_list_t list;
    linked_list_init(&list);

    insert_val(&list, 0, 0);
    insert_val(&list, 1, 1);
    insert_val(&list, 2, 2);
    insert_val(&list, 3, 3);

    // 0123=====3210
    print_validate(&list);
}

void insert_test_invalid()
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

    // 0123=====3210
    print_validate(&list);
}


void remove_test_beginning()
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
    print_validate(&list);
}

void remove_test_middle()
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
    print_validate(&list);
}

void remove_test_end()
{
    linked_list_t list;
    linked_list_init(&list);

    insert_val(&list, 0, 0);
    insert_val(&list, 1, 1);
    insert_val(&list, 2, 2);
    insert_val(&list, 3, 3);

    linked_list_remove_idx(&list, 3);
    linked_list_remove_idx(&list, 2);

    // 01=====10
    print_validate(&list);
}

void remove_test_all()
{
    linked_list_t list;
    linked_list_init(&list);

    printf("remove all:");

    insert_val(&list, 0, 0);
    insert_val(&list, 1, 1);
    insert_val(&list, 2, 2);
    insert_val(&list, 3, 3);

    linked_list_remove_idx(&list, 0);
    linked_list_remove_idx(&list, 0);
    linked_list_remove_idx(&list, 0);
    linked_list_remove_idx(&list, 0);

    printf(linked_list_size(&list) == 0 ? "   YEAH" : "   NO");
    validate_list(&list);

    insert_val(&list, 0, 0);
    insert_val(&list, 1, 1);
    insert_val(&list, 2, 2);
    insert_val(&list, 3, 3);

    linked_list_remove_idx(&list, 3);
    linked_list_remove_idx(&list, 2);
    linked_list_remove_idx(&list, 1);
    linked_list_remove_idx(&list, 0);

    printf(linked_list_size(&list) == 0 ? "   YEAH" : "   NO");
    validate_list(&list);

    insert_val(&list, 0, 0);
    insert_val(&list, 1, 1);
    insert_val(&list, 2, 2);
    insert_val(&list, 3, 3);

    linked_list_remove_idx(&list, 1);
    linked_list_remove_idx(&list, 1);
    linked_list_remove_idx(&list, 1);
    linked_list_remove_idx(&list, 0);

    printf(linked_list_size(&list) == 0 ? "   YEAH" : "   NO");
    validate_list(&list);

    printf("\n");
}

void remove_test_invalid()
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
    print_validate(&list);
}

void size_test()
{
    linked_list_t list;
    linked_list_init(&list);
    linked_list_t list2;
    linked_list_init(&list2);

    insert_val(&list, 0, 0);
    insert_val(&list, 1, 1);
    insert_val(&list, 2, 2);
    insert_val(&list, 3, 3);

    printf((linked_list_size(&list) == 4 && linked_list_size(&list2) == 0 && linked_list_size(NULL) == -1) ? "size:  okay" : "size:  oops");
    validate_list(&list);
    printf("\n");
}

void push_pop_front_test()
{
    linked_list_t list;
    linked_list_init(&list);

    push_front_val(&list, 1);
    push_front_val(&list, 2);
    push_front_val(&list, 3);
    push_front_val(&list, 4);

    //4321==1234
    print_validate(&list);

    linked_list_pop_front(&list);
    //321==123
    print_validate(&list);

    linked_list_pop_front(&list);
    linked_list_pop_front(&list);
    linked_list_pop_front(&list);

    printf((linked_list_size(&list) == 0 )? "    YEAH" : "    NO");
    validate_list(&list);
    printf("\n");
}

void push_pop_back_test()
{
    linked_list_t list;
    linked_list_init(&list);

    push_back_val(&list, 1);
    push_back_val(&list, 2);
    push_back_val(&list, 3);
    push_back_val(&list, 4);

    //1234==4321
    print_validate(&list);

    linked_list_pop_back(&list);
    //123==321
    print_validate(&list);

    linked_list_pop_back(&list);
    linked_list_pop_back(&list);
    linked_list_pop_back(&list);

    printf((linked_list_size(&list) == 0 )? "    YEAH" : "    NO");
    validate_list(&list);
    printf("\n");
}


int main(void)
{
    insert_test_beginning();
    insert_test_middle();
    insert_test_end();
    insert_test_invalid();

    remove_test_beginning();
    remove_test_middle();
    remove_test_end();
    remove_test_invalid();

    size_test();

    remove_test_all();

    push_pop_front_test();
    push_pop_back_test();

    return 0;
}

