#include "clib.h"
#include "cdef.h"
#include "lb/htable.h"
#include "lb/llist.h"

void
htable_init(struct htable *table, htable_cmp_fp cmp_fp, htable_hash_fp hash_fp, struct llist *buf, uint32 bkts)
{
    table->hash_fp = hash_fp;
    table->cmp_fp = cmp_fp;
    table->buf = buf;
    table->bkts = bkts;
    for (uint32 i = 0; i < bkts; i++)
    {
        lb_llist_init(&buf[i]);
    }
}

struct htable_node *
htable_insert(struct htable *table, struct htable_node *entry)
{
    uint32 hash = table->hash_fp(entry) % table->bkts;
    struct llist *hlist = &table->buf[hash];

    struct htable_node *ret = NULL;
    struct dlist_node *lnode = lb_llist_first(hlist);
    while(lnode != NULL)
    {
        struct htable_node *each = OBTAIN_STRUCT_ADDR(lnode, struct htable_node, list_node);
        if (table->cmp_fp(each, entry) == 0)
        {
            ret = each;
            break;
        }
        lnode = lb_llist_next(lnode);
    }

    if(ret != NULL)
    {
        lb_llist_remove(hlist, &ret->list_node);
    }

    /*
     * insert to the head
     */
    lb_llist_insert(hlist, NULL, &entry->list_node);

    return ret;
}