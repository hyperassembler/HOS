#include <kern/ref.h>
#include <kern/assert.h>
#include <kern/spin_lock.h>
#include <kern/atomic.h>
#include <kern/intr.h>
#include <kern/clib.h>

#define K_IDENT_BASE (0x80000000ul)

static struct a_tree ident_tree;
static struct spin_lock ident_tree_lock;
static uint32 ident_base;


static int32
handle_compare(struct atree_node *tree_node, struct atree_node *my_node)
{
    struct ident_node *tcb = OBTAIN_STRUCT_ADDR(tree_node, struct ident_node, tree_node);
    struct ident_node *my_tcb = OBTAIN_STRUCT_ADDR(my_node, struct ident_node, tree_node);

    if ((uintptr) tcb->ident > (uintptr) my_tcb->ident)
    {
        return -1;
    }
    else
    {
        if ((uintptr) tcb->ident == (uintptr) my_tcb->ident)
        {
            return 0;
        }
        else
        {
            return 1;
        }
    }
}


static struct ident_node *
search_handle_node(k_ident ident)
{
    struct atree_node *result;
    struct ident_node temp;
    temp.ident = ident;
    result = lb_atree_search(&ident_tree, &temp.tree_node);
    return result == NULL ? NULL : OBTAIN_STRUCT_ADDR(result, struct ident_node, tree_node);
}


k_status
rf_ref_init(void)
{
    lb_atree_init(&ident_tree, handle_compare);
    ke_spin_init(&ident_tree_lock);
    ident_base = K_IDENT_BASE;

    return STATUS_SUCCESS;
}


k_status
rf_ref_node_init(struct ref_node *rf_node, ref_free_fp free_func)
{
    KE_ASSERT(ke_get_irql() <= IRQL_DPC);

    rf_node->f_free = free_func;
    rf_node->ref_count = 1;

    return STATUS_SUCCESS;
}


k_status
rf_ref_obj(struct ref_node *ref_node)
{
    KE_ASSERT(ke_get_irql() <= IRQL_DPC);

    if (ref_node == NULL)
    {
        return STATUS_INVALID_ARGS;
    }

    int32 old_ref_count = ke_atomic_inc_32(&ref_node->ref_count, 1);

    KE_ASSERT(old_ref_count >= 1);

    return STATUS_SUCCESS;
}


k_status
rf_deref_obj(struct ref_node *rf_node)
{
    KE_ASSERT(ke_get_irql() <= IRQL_DPC);

    if (rf_node == NULL)
    {
        return STATUS_INVALID_ARGS;
    }

    k_status result = STATUS_SUCCESS;

    int32 old_ref_count = ke_atomic_inc_32(&rf_node->ref_count, -1);

    KE_ASSERT(old_ref_count >= 1);

    if (old_ref_count == 1)
    {
        rf_node->f_free(rf_node);
    }

    return result;
}


k_status
rf_open_obj_by_ident(k_ident id, struct ref_node **out)
{
    KE_ASSERT(ke_get_irql() <= IRQL_DPC);

    uint32 irql;
    k_status status = STATUS_SUCCESS;
    struct ref_node *ref = NULL;

    irql = ke_raise_irql(IRQL_DPC);
    ke_spin_lock(&ident_tree_lock);
    struct ident_node *ident_node = search_handle_node(id);
    if (ident_node == NULL)
    {
        status = STATUS_INVALID_ARGS;
    }
    else
    {
        ref = ident_node->obj;
    }

    // PREREQUISITE: Having a handle -> having a reference
    // MUST GUARANTEE that handle exists while we reference
    if (SX_SUCCESS(status))
    {
        // reference the object then return the reference
        rf_ref_obj(ref);
        *out = ref;
    }

    ke_spin_unlock(&ident_tree_lock);
    ke_lower_irql(irql);

    return status;
}


k_status
rf_ident_create(struct ref_node *rf_node, struct ident_node *id_node, k_ident *out)
{
    k_status result;
    uint32 irql;

    KE_ASSERT(ke_get_irql() <= IRQL_DPC);

    result = STATUS_SUCCESS;

    // TODO: CHECK OVERFLOW
    id_node->ident = (k_ident) ke_atomic_inc_32((int32 *) &ident_base, 1);
    id_node->obj = rf_node;

    irql = ke_raise_irql(IRQL_DPC);
    ke_spin_lock(&ident_tree_lock);

    struct ident_node *existing_node = search_handle_node(id_node->ident);
    if (existing_node == NULL)
    {
        lb_atree_insert(&ident_tree, &id_node->tree_node);
    }
    else
    {
        /**
         * Shouldn't get to here
         */
        result = STATUS_DUPLICATE;
    }

    ke_spin_unlock(&ident_tree_lock);
    ke_lower_irql(irql);


    if (SX_SUCCESS(result))
    {
        rf_ref_obj(rf_node);
        *out = id_node->ident;
    }

    return result;
}


k_status
rf_ident_close(k_ident id)
{
    uint32 irql;
    k_status status;
    struct ref_node *ref;

    KE_ASSERT(ke_get_irql() <= IRQL_DPC);

    status = STATUS_SUCCESS;
    ref = NULL;

    irql = ke_raise_irql(IRQL_DPC);
    ke_spin_lock(&ident_tree_lock);

    struct ident_node *id_node = search_handle_node(id);
    if (id_node == NULL)
    {
        status = STATUS_INVALID_ARGS;
    }
    else
    {
        ref = id_node->obj;
        lb_atree_delete(&ident_tree, &id_node->tree_node);
    }

    ke_spin_unlock(&ident_tree_lock);
    ke_lower_irql(irql);

    if (SX_SUCCESS(status))
    {
        /**
         * Success means already freed
         */
        id_node->free_routine(id_node);
        /**
         * Dereference the object
         */
        rf_deref_obj(ref);
    }

    return status;
}
