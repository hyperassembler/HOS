#ifndef _LINKED_LIST_H_
#define _LINKED_LIST_H_
#include "../../../sys/kdef.h"
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

void _KERNEL_ABI linked_list_init(linked_list_t * list);

void _KERNEL_ABI linked_list_push_back(linked_list_t *list, linked_list_node_t *node);

void _KERNEL_ABI linked_list_insert(linked_list_t * list, int index, linked_list_node_t * node);

linked_list_node_t *_KERNEL_ABI linked_list_get(linked_list_t * list, int index);

void _KERNEL_ABI linked_list_remove(linked_list_t *list, int index);

void _KERNEL_ABI linked_list_node_init(linked_list_node_t * node);
#endif
