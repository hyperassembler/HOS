#pragma once

#include "cdef.h"

struct llist_node
{
    struct llist_node *prev;
    struct llist_node *next;
};

struct llist
{
    struct llist_node *head;
    struct llist_node *tail;
    uint32 size;
};

void
lb_llist_init(struct llist *list);

uint32
lb_llist_size(struct llist *list);

void
lb_llist_push_front(struct llist *list, struct llist_node *node);

void
lb_llist_push_back(struct llist *list, struct llist_node *node);

struct llist_node *
lb_llist_pop_front(struct llist *list);


struct llist_node *
lb_llist_pop_back(struct llist *list);

void
lb_llist_insert_by_idx(struct llist *list, uint32 index, struct llist_node *node);

struct llist_node *
lb_llist_remove_by_idx(struct llist *list, uint32 index);


struct llist_node *
lb_llist_get(struct llist *list, uint32 index);


void
lb_llist_insert_by_ref(struct llist *list, struct llist_node *cur_node, struct llist_node *new_node);


struct llist_node *
lb_llist_remove_by_ref(struct llist *list, struct llist_node *node);


struct llist_node *
lb_llist_next(struct llist_node *node);


struct llist_node *
lb_llist_prev(struct llist_node *node);


struct llist_node *
lb_llist_first(struct llist *list);


struct llist_node *
lb_llist_last(struct llist *list);
