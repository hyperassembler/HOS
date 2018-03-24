#ifndef LIB_LINKED_LIST_H
#define LIB_LINKED_LIST_H

#include "type.h"

struct linked_list_node
{
	struct linked_list_node *prev;
	struct linked_list_node *next;
};

struct linked_list
{
	struct linked_list_node *head;
	struct linked_list_node *tail;
};

/*
 * Returns true if current list node == your node
 * false otherwise
 */

void SXAPI lb_linked_list_init(struct linked_list *list);

int32 SXAPI lb_linked_list_size(struct linked_list *list);

void SXAPI lb_linked_list_push_front(struct linked_list *list, struct linked_list_node *node);

void SXAPI lb_linked_list_push_back(struct linked_list *list, struct linked_list_node *node);

struct linked_list_node *SXAPI lb_linked_list_pop_front(struct linked_list *list);

struct linked_list_node *SXAPI lb_linked_list_pop_back(struct linked_list *list);

void SXAPI lb_linked_list_insert(struct linked_list *list, int32 index, struct linked_list_node *node);

void SXAPI lb_linked_list_insert_ref(struct linked_list *list, struct linked_list_node *prev_node, struct linked_list_node *node);

struct linked_list_node *SXAPI lb_linked_list_remove(struct linked_list *list, int32 index);

struct linked_list_node *SXAPI lb_linked_list_remove_ref(struct linked_list *list, struct linked_list_node *node);

struct linked_list_node *SXAPI lb_linked_list_get(struct linked_list *list, int32 index);

struct linked_list_node *SXAPI lb_linked_list_next(struct linked_list_node *node);

struct linked_list_node *SXAPI lb_linked_list_prev(struct linked_list_node *node);

struct linked_list_node *SXAPI lb_linked_list_first(struct linked_list *list);

struct linked_list_node *SXAPI lb_linked_list_last(struct linked_list *list);

int32 SXAPI lb_linked_list_search(struct linked_list *list, struct linked_list_node *target,
                                   callback_func equals);

#endif
