#include "cdef.h"
#include "dlist.h"

struct htable_node;

typedef uint32 (*htable_cmp_fp)(struct htable_node* table_node, struct htable_node* self);

typedef uint32 (*htable_hash_fp)(struct htable_node* self);

struct htable_node
{
    struct dlist_node list_node;
};

struct htable
{
    uint32 bkts;
    struct dlist *buf;
    htable_cmp_fp cmp_fp;
    htable_hash_fp hash_fp;
};

void
htable_init(struct htable* table, htable_cmp_fp cmp_fp, htable_hash_fp hash_fp, struct dlist *buf, uint32 bkts);

/**
 * returns the overwritten object
 * returns NULL if no duplicates are overwritten
 */
struct htable_node*
htable_insert(struct htable* table, struct htable_node* entry);


/**
 * returns the removed node
 * NULL if doesn't exist
 */
struct htable_node*
htable_remove(struct htable* table, struct htable_node* entry);


struct htable_node*
htable_contains(struct htable* table, struct htable_node* entry);
