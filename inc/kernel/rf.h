#pragma once

#include "common.h"
#include "kernel/status.h"
#include "kernel/lb.h"

typedef uint32 k_ident;

typedef void (*ref_free_fp)(void *);

struct ref_node
{
    int32 ref_count;
    ref_free_fp f_free;
};

struct ident_node
{
    struct atree_node tree_node;
    k_ident ident;
    struct ref_node *obj;
    ref_free_fp free_routine;
};

//
// All functions are sx since users or kernel devs should not be
// specifying where the allocations take place
//

k_status
rf_ref_init(void);

k_status
rf_ref_node_init(struct ref_node *rf_node, ref_free_fp free_func);

k_status
rf_ref_obj(struct ref_node *rf_node);

k_status
rf_deref_obj(struct ref_node *rf_node);

k_status
rf_open_obj_by_ident(k_ident id, struct ref_node **out);

k_status
rf_ident_create(struct ref_node *rf_node, struct ident_node *id_node, k_ident *out);

k_status
rf_ident_close(k_ident id);

