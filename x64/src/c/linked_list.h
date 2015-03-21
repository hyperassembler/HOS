#ifndef _LINKED_LIST_H_
#define _LINKED_LIST_H_
#include "kdef.h"
typedef struct _linked_list_node
{
    struct _linked_list_node* prev;
    struct _linked_list_node* next;
} linked_list_node;

typedef struct
{
    linked_list_node* head;
    int size;
} linked_list;

void NATIVE64 linked_list_init(linked_list* list);

void NATIVE64 linked_list_add(linked_list * list, linked_list_node* node);

void NATIVE64 linked_list_insert(linked_list * list, int index, linked_list_node* node);

linked_list_node* NATIVE64 linked_list_get(linked_list * list, int index);

void NATIVE64 linked_list_remove(linked_list *list, int index);



int NATIVE64 linked_list_node_size(linked_list_node* head);

void NATIVE64 linked_list_node_init(linked_list_node* node);

void NATIVE64 linked_list_node_add(linked_list_node * head, linked_list_node* node);

linked_list_node* NATIVE64 linked_list_node_insert(linked_list_node * head, int index, linked_list_node* node);

linked_list_node* NATIVE64 linked_list_node_get(linked_list_node * head, int index);

linked_list_node* NATIVE64 linked_list_node_remove(linked_list_node *head, int index);

#endif
