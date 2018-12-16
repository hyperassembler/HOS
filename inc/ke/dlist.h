#pragma once

#include "cdef.h"

struct dlist_node
{
    struct dlist_node *prev;
    struct dlist_node *next;
};

struct dlist
{
    struct dlist_node *head;
    struct dlist_node *tail;
};

void
lb_dlist_init(struct dlist *list);

void
lb_dlist_insert(struct dlist *list, struct dlist_node *cur_node, struct dlist_node *new_node);


struct dlist_node *
lb_dlist_remove(struct dlist *list, struct dlist_node *node);


struct dlist_node *
lb_dlist_next(struct dlist_node *node);


struct dlist_node *
lb_dlist_prev(struct dlist_node *node);


struct dlist_node *
lb_dlist_first(struct dlist *list);


struct dlist_node *
lb_dlist_last(struct dlist *list);
