#include <stdio.h>
#include <assert.h>
#include "linked_list.h"

void print_list(linked_list* list)
{
    linked_list_iterator* it = linked_list_create_iterator(list);
    it->current = list->tail;
    while(it->current)
    {
        printf("%lu ", (unsigned long)it->current->data);
        linked_list_prev(it);
    }
    printf("\n");
}

void insert_test(void)
{
    printf("insert test\n");
    linked_list* list;
    //empty list at test
    list = linked_list_create();
    linked_list_insert_at(list,0,(void*)2);
    assert(list->size == 1 && list->head == list->tail && (int)list->head->data == 2);
    print_list(list);

    //tail test
    linked_list_insert_at(list,1,(void*)3);
    assert(list->size == 2 && (int)list->tail->data == 3);
    print_list(list);

    //head test
    linked_list_insert_at(list,0,(void*)1);
    assert(list->size == 3 && (int)list->head->data == 1);
    print_list(list);

    //invalid index test
    linked_list_insert_at(list,4,(void*)1);
    assert(list->size == 3 && (int)list->head->data == 1 && (int)list->tail->data == 3);
    print_list(list);

    //normal test
    linked_list_insert_at(list,1,(void*)5);
    assert(list->size == 4 && (int)list->head->next->data == 5 && (int)list->head->next->next->data == 2);
    print_list(list);

    linked_list_free(list,NULL);

    return;
}

void delete_test(void)
{
    printf("delete test\n");
    linked_list* list;
    //empty list at test
    list = linked_list_create();
    linked_list_insert(list,(void*)1);
    linked_list_insert(list,(void*)2);
    linked_list_insert(list,(void*)3);
    linked_list_insert(list,(void*)4);
    linked_list_insert(list,(void*)5);

    //head test
    linked_list_delete_at(list,0);
    assert(list->size == 4 && (int)list->head->data == 2);
    print_list(list);

    //tail test
    linked_list_delete_at(list,list->size-1);
    assert(list->size == 3 && (int)list->tail->data == 4);
    print_list(list);

    //normal test
    linked_list_delete_at(list,1);
    assert(list->size == 2 && (int)list->head->data == 2 && (int)list->head->next->data==4);
    print_list(list);

    //invalid index test
    linked_list_delete_at(list,2);
    assert(list->size == 2 && (int)list->head->data == 2 && (int)list->head->next->data==4);
    print_list(list);

    //delete last test
    linked_list_delete_at(list,0);
    linked_list_delete_at(list,0);
    assert(list->size == 0 && list->head == NULL && list->tail == NULL);

    linked_list_free(list,NULL);

    return;
}

int main(void)
{
    insert_test();
    delete_test();
    return 0;
}