#ifndef _LINKED_LIST_H_
#define _LINKED_LIST_H_
typedef struct _linked_list_node
{
    struct _linked_list_node* prev;
    struct _linked_list_node* next;
    void* data;
} linked_list_node;

typedef struct
{
    linked_list_node* head;
    linked_list_node* tail;
    int size;
} linked_list;

typedef struct
{
    linked_list_node* current;
} linked_list_iterator;

linked_list* linked_list_create();

void linked_list_insert(linked_list * list, void* data);

void linked_list_insert_at(linked_list * list, int position, void* data);

void* linked_list_get(linked_list * list, int index);

void linked_list_delete(linked_list * list, void* data, int(*compare)(int*,int*));

void linked_list_delete_at(linked_list * list, int index);

linked_list_iterator* linked_list_create_iterator(linked_list* list);

void linked_list_delete_iterator(linked_list_iterator* it);

void linked_list_prev(linked_list_iterator* it);

void linked_list_next(linked_list_iterator* it);

void linked_list_free(linked_list* list, void(*delete_data)(void*));

#endif
