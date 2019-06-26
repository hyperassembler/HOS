#pragma once

#include <kern/cdef.h>
#include <kern/assert.h>

struct list_entry {
    struct list_entry *next;
    struct list_entry *prev;
};

struct list {
    struct list_entry *head;
    struct list_entry *tail;
};

/*
 * Init Operations
 */
static inline void
list_init(struct list *list)
{
    list->head = NULL;
    list->tail = NULL;
}

static inline void
list_entry_init(struct list_entry *ent)
{
    ent->prev = NULL;
    ent->next = NULL;
}

static inline bool
list_empty(struct list_entry *ent)
{
    return (ent->next == NULL);
}
/*
 * Location Operations
 */
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

static inline struct list_entry *
list_head(struct list *list)
{
    return list->head;
}

static inline struct list_entry *
list_tail(struct list *list)
{
    return list->tail;
}

/*
 * Insert Operations
 */
void
list_insert(struct list *list, struct list_entry *cur, struct list_entry *ent);

static inline void
list_insert_head(struct list *list, struct list_entry *ent)
{
    list_insert(list, NULL, ent);
}

static inline void
list_insert_tail(struct list *list, struct list_entry *ent)
{
    list_insert(list, list_tail(list), ent);
}

/*
 * Remove Operations
 */
void
list_remove(struct list *list, struct list_entry *ent);


static inline struct list_entry *
list_remove_tail(struct list *list)
{
    struct list_entry *ret = list_tail(list);
    list_remove(list, ret);
    return ret;
}

static inline struct list_entry *
list_remove_head(struct list *list)
{
    struct list_entry *ret = list_head(list);
    list_remove(list, ret);
    return ret;
}
