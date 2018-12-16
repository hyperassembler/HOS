#pragma once

#include "cdef.h"

struct slist_node
{
    struct slist_node *next;
};

struct slist
{
    struct slist_node *head;
    struct slist_node *tail;
};

void
lb_slist_init(struct slist *list);

void
lb_slist_insert(struct slist *list, struct slist_node *cur_node, struct slist_node *new_node);


struct dlist_node *
lb_slist_remove(struct slist *list, struct slist_node *node);


struct dlist_node *
lb_slist_next(struct slist_node *node);


struct dlist_node *
lb_slist_first(struct slist *list);

