#pragma once

#include <kern/cdef.h>
#include <kern/poison.h>
#include <kern/brute.h>

struct list_entry {
    struct list_entry *prev;
    struct list_entry *next;
};

/*
 * Init Operations
 */
static inline void
list_init(struct list_entry *head)
{
    head->next = head;
    head->prev = head;
}

static inline int
list_empty(struct list_entry *ent)
{
    return (ent->next == ent);
}

static inline struct list_entry *
list_prev(struct list_entry *ent)
{
    return ent->prev;
}

static inline struct list_entry *
list_next(struct list_entry *ent)
{
    return ent->next;
}

/*
 * Insert Operations
 */
static inline void 
list_insert(struct list_entry *head, struct list_entry *ent)
{
    ent->next = head->next;
    ent->prev = head;

    head->next->prev = ent;
    head->next = ent;
}

static inline void
list_insert_before(struct list_entry *head, struct list_entry *ent)
{
    list_insert(head->prev, ent);
}

/*
 * Remove Operations
 */
static inline struct list_entry *
list_remove(struct list_entry *ent)
{
    ent->next->prev = ent->prev;
    ent->prev->next = ent->next;
    
    ent->next = POISON_LIST;
    ent->prev = POISON_LIST;

    return ent;
}

static inline struct list_entry *
list_remove_before(struct list_entry *list)
{
    return list_remove(list->prev);
}

static inline struct list_entry *
list_remove_after(struct list_entry *list)
{
    return list_remove(list->next);
}

#define LIST_FOREACH(list, it) \
    for (it = list_next(list); it != list; it = list_next(it))

#define LIST_FOREACH_REVERSE(list, it) \
    for (it = list_prev(list); it != list; it = list_prev(it))
