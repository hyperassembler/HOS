#ifndef _LINKED_LIST_H_
#define _LINKED_LIST_H_
#include "../../../kdef.h"
typedef struct _linked_list_node_t
{
    struct _linked_list_node_t * prev;
    struct _linked_list_node_t * next;
} linked_list_node_t;

typedef struct
{
    linked_list_node_t * head;
    int size;
} linked_list_t;

void NATIVE64 linked_list_init(linked_list_t * list);

void NATIVE64 linked_list_add(linked_list_t * list, linked_list_node_t * node);

void NATIVE64 linked_list_insert(linked_list_t * list, int index, linked_list_node_t * node);

linked_list_node_t * NATIVE64 linked_list_get(linked_list_t * list, int index);

void NATIVE64 linked_list_remove(linked_list_t *list, int index);

int NATIVE64 linked_list_node_size(linked_list_node_t * head);

void NATIVE64 linked_list_node_init(linked_list_node_t * node);

void NATIVE64 linked_list_node_add(linked_list_node_t * head, linked_list_node_t * node);

linked_list_node_t * NATIVE64 linked_list_node_insert(linked_list_node_t * head, int index, linked_list_node_t * node);

linked_list_node_t * NATIVE64 linked_list_node_get(linked_list_node_t * head, int index);

linked_list_node_t * NATIVE64 linked_list_node_remove(linked_list_node_t *head, int index);

#endif
