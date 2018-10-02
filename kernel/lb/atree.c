#include "lb/atree.h"
#include "clib.h"

static struct atree_node *
atree_node_max(struct atree_node *node)
{
    while ((node != NULL) && (node->right != NULL))
    {
        node = node->right;
    }
    return node;
}


static struct atree_node *
atree_node_min(struct atree_node *node)
{
    while ((node != NULL) && (node->left != NULL))
    {
        node = node->left;
    }
    return node;
}


static void
atree_node_init(struct atree_node *it)
{
    if (it != NULL)
    {
        it->height = 0;
        it->left = NULL;
        it->right = NULL;
    }
}


static int32
atree_node_get_height(struct atree_node *node)
{
    return node == NULL ? -1 : node->height;
}


static int32
atree_node_get_balance_factor(struct atree_node *node)
{
    if (node == NULL)
    {
        return 0;
    }
    return atree_node_get_height(node->left) - atree_node_get_height(node->right);
}


static struct atree_node *
atree_node_right_rotate(struct atree_node *node)
{
    struct atree_node *lchild = node->left;
    node->left = lchild->right;
    lchild->right = node;

    node->height = MAX(atree_node_get_height(node->left), atree_node_get_height(node->right)) + 1;
    lchild->height = MAX(atree_node_get_height(lchild->left), atree_node_get_height(lchild->right)) + 1;
    return lchild;
}


static struct atree_node *
atree_node_left_rotate(struct atree_node *node)
{
    struct atree_node *rchild = node->right;
    node->right = rchild->left;
    rchild->left = node;

    node->height = MAX(atree_node_get_height(node->left), atree_node_get_height(node->right)) + 1;
    rchild->height = MAX(atree_node_get_height(rchild->left), atree_node_get_height(rchild->right)) + 1;
    return rchild;
}


static struct atree_node *
atree_node_balance(struct atree_node *node)
{
    int32 bf;
    int32 cbf;

    bf = atree_node_get_balance_factor(node);

    if (bf > 1)
    {
        /**
         * Left double heavy
         */
        cbf = atree_node_get_balance_factor(node->left);
        if (cbf >= 0)
        {
            /**
             *
             * Left child is left heavy
             *            x (k)                                 y (k-1)
             *           / \               RR(x)               / \
             *    (k-1) y   A (k-3)     ----------->     (k-2)B   x (k-2)
             *         / \                                       / \
             *  (k-2) B   C (k-3)                         (k-3) C   A (k-3)
             */
            return atree_node_right_rotate(node);
        }
        else
        {
            /**
            *
            * Left child is right heavy
            *            x (k)                                          x (k)
            *           / \                                            / \
            *    (k-1) y   A (k-3)         LR(y)               (k-1)  z   A (k-3)
            *         / \              ------------>                 / \
            *  (k-3) B   z (k-2)                              (k-2) y   D (k-4)
            *           / \                                        / \
            *    (k-3) C   D (k-4)                         (k-3)  B   C (k-3)
            *
            *
            *            x (k)                                          __z__  (k-1)
            *           / \                                            /     \
            *  (k-1)   z   A (k-3)                             (k-2)  y       x  (k-2)
            *         / \                 RR(x)                      /  \    /  \
            *  (k-2) y   D (k-4)       ------------>                B    C  D    A
            *       / \
            *  (k-3)B  C (k-3)
            */
            node->left = atree_node_left_rotate(node->left);
            return atree_node_right_rotate(node);
        }
    }
    else if (bf < -1)
    {
        {
            cbf = atree_node_get_balance_factor(node->right);
            if (cbf <= 0)
            {
                // right right, see above
                return atree_node_left_rotate(node);
            }
            else
            {
                // right left, see above
                node->right = atree_node_right_rotate(node->right);
                return atree_node_left_rotate(node);
            }
        }
    }
    else
    {
        return node;
    }
}


static struct atree_node *
atree_node_insert(struct atree_node *node, struct atree_node *entry, atree_cmp_fp compare, struct atree_node **overwritten)
{
    if (node == NULL)
    {
        atree_node_init(entry);
        return entry;
    }

    int32 comp = compare(node, entry);
    if (comp < 0)
    {
        node->right = atree_node_insert(node->right, entry, compare, overwritten);
    }
    else
    {
        if (comp == 0)
        {
            /**
             * overwrite existing value
             */
            atree_node_init(entry);
            entry->right = node->right;
            entry->left = node->left;
            entry->height = node->height;
            *overwritten = node;
            return entry;
        }
        else
        {
            node->left = atree_node_insert(node->left, entry, compare, overwritten);
        }
    }

    node->height = MAX(atree_node_get_height(node->left), atree_node_get_height(node->right)) + 1;

    return atree_node_balance(node);
}


static struct atree_node *
atree_node_search(struct atree_node *node, struct atree_node *entry, atree_cmp_fp compare, struct atree_node **parent)
{
    int32 comp;
    struct atree_node *prev;
    struct atree_node *temp;

    prev = NULL;

    while (node != NULL)
    {
        comp = compare(node, entry);
        temp = node;
        if (comp < 0)
        {
            node = node->right;
        }
        else if (comp > 0)
        {
            node = node->left;
        }
        else
        {
            break;
        }

        prev = temp;
    }

    if (parent != NULL)
    {
        *parent = prev;
    }

    return node;
}


static struct atree_node *
atree_node_delete(struct atree_node *node, struct atree_node *entry, atree_cmp_fp compare, struct atree_node **deleted)
{
    int32 comp;
    struct atree_node *succ_parent;

    if (node == NULL)
    {
        return NULL;
    }

    comp = compare(node, entry);
    if (comp < 0)
    {
        node->right = atree_node_delete(node->right, entry, compare, deleted);
    }
    else if (comp > 0)
    {
        node->left = atree_node_delete(node->left, entry, compare, deleted);
    }
    else
    {
        /**
         * Write the deleted node first
         */
        *deleted = node;

        if ((node->left == NULL) || (node->right == NULL))
        {
            /**
            * 0 or 1 child
            */
            struct atree_node *child = node->left != NULL ? node->left : node->right;

            if (child == NULL)
            {
                node = NULL;
            }
            else
            {
                node = child;
            }
        }
        else
        {
            /**
             * 2 children
             * meaning that the successor must be in the right subtree
            */
            struct atree_node *succ = atree_node_min(node->right);
            atree_node_search(node, succ, compare, &succ_parent);

            /**
             * Swap the nodes
             * note that after swapping, the BST property of the right subtree is preserved
             */
            if (succ_parent == node)
            {
                /**
                 * check special case where the successor is the right child
                 */
                node->right = succ->right;
                succ->right = node;
            }
            else
            {
                if (succ_parent->left == succ)
                {
                    succ_parent->left = node;
                }
                else
                {
                    succ_parent->right = node;
                }
                SWAP(&node->right, &succ->right, struct atree_node*);
            }
            SWAP(&node->left, &succ->left, struct atree_node*);
            SWAP(&node->height, &succ->height, int32);

            /**
             * Delete the node from the right subtree
             */
            succ->right = atree_node_delete(succ->right, node, compare, deleted);

            node = succ;
        }
    }

    /**
     * balance the new head
     */
    if (node != NULL)
    {
        node->height = MAX(atree_node_get_height(node->left), atree_node_get_height(node->right)) + 1;
        node = atree_node_balance(node);
    }

    return node;
}


struct atree_node *
lb_atree_min(struct atree *tree)
{
    return atree_node_min(tree->root);
}


struct atree_node *
lb_atree_max(struct atree *tree)
{
    return atree_node_max(tree->root);
}


struct atree_node *
lb_atree_next(struct atree *tree, struct atree_node *entry)
{
    struct atree_node *succ;
    struct atree_node *node;
    int32 comp;

    if (entry->right != NULL)
    {
        succ = atree_node_min(entry->right);
    }
    else
    {
        succ = NULL;
        node = tree->root;

        while (node != NULL)
        {
            comp = tree->cmpf(node, entry);

            if (comp < 0)
            {
                node = node->right;
            }
            else if (comp > 0)
            {
                succ = node;
                node = node->left;
            }
            else
            {
                break;
            }
        }
    }

    return succ;
}


struct atree_node *
lb_atree_prev(struct atree *tree, struct atree_node *entry)
{
    struct atree_node *prev;
    struct atree_node *node;
    int32 comp;

    if (entry->left != NULL)
    {
        prev = atree_node_max(entry->left);
    }
    else
    {
        prev = NULL;
        node = tree->root;

        while (node != NULL)
        {
            comp = tree->cmpf(node, entry);

            if (comp < 0)
            {
                prev = node;
                node = node->right;
            }
            else if (comp > 0)
            {
                node = node->left;
            }
            else
            {
                break;
            }
        }
    }

    return prev;
}


struct atree_node *
lb_atree_search(struct atree *tree, struct atree_node *entry)
{
    return atree_node_search(tree->root, entry, tree->cmpf, NULL);
}


struct atree_node *
lb_atree_insert(struct atree *tree, struct atree_node *entry)
{
    struct atree_node *old;

    old = NULL;
    tree->root = atree_node_insert(tree->root, entry, tree->cmpf, &old);
    return old;
}


struct atree_node *
lb_atree_delete(struct atree *tree, struct atree_node *entry)
{
    struct atree_node *node;

    node = NULL;
    tree->root = atree_node_delete(tree->root, entry, tree->cmpf, &node);
    return node;
}


uint32
lb_atree_size(struct atree *tree)
{
    uint32 size;
    struct atree_node *node;

    size = 0;
    if (tree->root != NULL)
    {
        node = lb_atree_min(tree);
        while (node != NULL)
        {
            size++;
            node = lb_atree_next(tree, node);
        }
    }
    return size;
}


void
lb_atree_init(struct atree *tree, atree_cmp_fp compare)
{
    tree->cmpf = compare;
    tree->root = NULL;
}


/**
 * Used by tests
 */

static
int32
atree_node_calc_height(struct atree_node *tree)
{
    if (tree == NULL)
    {
        return -1;
    }
    return MAX(atree_node_calc_height(tree->left), atree_node_calc_height(tree->right)) + 1;
}


static
bool
atree_node_test(struct atree_node *tree, atree_cmp_fp compare)
{
    if (tree == NULL)
    {
        return TRUE;
    }

    if (atree_node_get_balance_factor(tree) < -1 || atree_node_get_balance_factor(tree) > 1 ||
        atree_node_calc_height(tree) != tree->height)
    {
        return FALSE;
    }

    if(tree->height == 0 && ((tree->left != NULL) || (tree->right != NULL)))
    {
        return FALSE;
    }

    if(tree->right == tree || tree->left == tree || (tree->right == tree->left && tree->right != NULL))
    {
        return FALSE;
    }

    if ((tree->right != NULL && compare(tree, tree->right) > 0) ||
        (tree->left != NULL && compare(tree, tree->left) < 0))
    {
        return FALSE;
    }

    return atree_node_test(tree->left, compare) && atree_node_test(tree->right, compare);
}


bool
lb_atree_validate(struct atree *tree)
{
    if (tree == NULL)
    {
        return TRUE;
    }
    return atree_node_test(tree->root, tree->cmpf);
}
