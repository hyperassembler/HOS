#include <kern/list.h>
#include <kern/cdef.h>

void
list_insert(struct list *list, struct list_entry *cur, struct list_entry *ent)
{
    struct list_entry *left_ent;
    struct list_entry *right_ent;

    /*
     * adjust the current entry
     */
    if (cur == NULL) {
        ent->next = list->head;
        ent->prev = NULL;
    } else {
        ent->prev = cur;
        ent->next = cur->next;
    }

    /*
     * make left and right entry point at correct things
     */
    left_ent = cur;
    right_ent = cur == NULL ? list->head : cur->next;

    /*
     * adjust left and treenode node accordingly
     */
    if (left_ent != NULL) {
        left_ent->next = ent;
    } else {
        list->head = ent;
    }

    if (right_ent != NULL) {
        right_ent->prev = ent;
    } else {
        list->tail = ent;
    }
}

void
list_remove(struct list *list, struct list_entry *ent)
{
    if (ent->prev != NULL) {
        ent->prev->next = ent->next;
    } else {
        list->head = ent->next;
    }

    if (ent->next != NULL) {
        ent->next->prev = ent->prev;
    } else {
        list->tail = ent->prev;
    }
}

