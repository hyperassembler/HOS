#include <common/avl_tree.h>
#include <common/cdef.h>

#include <common/libkern.h>
#include <ke/poison.h>

static inline struct avl_node *
_avl_node_max(struct avl_node *node)
{
    while (node != NULL) {
        node = node->right;
    }
    return node;
}

/* the minimum in the subtree with parent */
static inline struct avl_node *
_avl_node_minp(struct avl_node *node, struct avl_node **parent)
{
    struct avl_node *p = NULL;

    while (node != NULL) {
        p = node;
        node = node->left;
    }

    if (parent != NULL) {
        *parent = p;
    }
    return node;
}

static inline struct avl_node *
_avl_node_min(struct avl_node *node)
{
    return _avl_node_minp(node, NULL);
}

static inline void
_avl_node_poison(struct avl_node *node)
{
    node->left = POISON_AVL;
    node->right = POISON_AVL;
}

static inline int
_avl_node_height(struct avl_node *node)
{
    return node == NULL ? -1 : node->height;
}


static inline int
_avl_node_bf(struct avl_node *node)
{
    if (node == NULL) {
        return 0;
    }
    return _avl_node_height(node->left) - _avl_node_height(node->right);
}


static inline struct avl_node *
_avl_node_rr(struct avl_node *node)
{
    struct avl_node *lchild = node->left;
    node->left = lchild->right;
    lchild->right = node;

    node->height = MAX(_avl_node_height(node->left), _avl_node_height(node->right)) + 1;
    lchild->height = MAX(_avl_node_height(lchild->left), _avl_node_height(lchild->right)) + 1;
    return lchild;
}


static inline struct avl_node *
_avl_node_lr(struct avl_node *node)
{
    struct avl_node *rchild = node->right;
    node->right = rchild->left;
    rchild->left = node;

    node->height = MAX(_avl_node_height(node->left), _avl_node_height(node->right)) + 1;
    rchild->height = MAX(_avl_node_height(rchild->left), _avl_node_height(rchild->right)) + 1;
    return rchild;
}

/* balances the subtree, returns the new root of the subtree */
static struct avl_node *
_avl_node_balance(struct avl_node *node)
{
    int bf;
    int cbf;

    bf = _avl_node_bf(node);

    if (bf > 1) {
        /*
         * Left double heavy
         */
        cbf = _avl_node_bf(node->left);
        if (cbf >= 0) {
            /*
             *
             * Left child is left heavy
             *            x (k)                                 y (k-1)
             *           / \               RR(x)               / \
             *    (k-1) y   A (k-3)     ----------->     (k-2)B   x (k-2)
             *         / \                                       / \
             *  (k-2) B   C (k-3)                         (k-3) C   A (k-3)
             */
            return _avl_node_rr(node);
        } else {
            /*
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
            node->left = _avl_node_lr(node->left);
            return _avl_node_rr(node);
        }
    } else if (bf < -1) {
        {
            cbf = _avl_node_bf(node->right);
            if (cbf <= 0) {
                // right right, see above
                return _avl_node_lr(node);
            } else {
                // right left, see above
                node->right = _avl_node_rr(node->right);
                return _avl_node_lr(node);
            }
        }
    } else {
        return node;
    }
}

/* does not allow duplicates, returns the new root of the subtree after insertion */
static struct avl_node *
_avl_node_insert(struct avl_node *node, struct avl_node *entry, avl_cmpf cmpf, struct avl_node **out)
{
    if (node == NULL) {
        /* leaf */
        entry->height = 0;
        entry->left = NULL;
        entry->right = NULL;
        *out = entry;
        return entry;
    }

    int comp = cmpf(node, entry);
    if (comp < 0) {
        node->right = _avl_node_insert(node->right, entry, cmpf, out);
    } else if (comp == 0) {
        /* duplicate */
        *out = NULL;
        return node;
    } else {
        node->left = _avl_node_insert(node->left, entry, cmpf, out);
    }

    node->height = MAX(_avl_node_height(node->left), _avl_node_height(node->right)) + 1;

    return _avl_node_balance(node);
}

/* find the node and its parent in the subtree */
static struct avl_node *
_avl_node_search(struct avl_node *node, struct avl_node *entry, avl_cmpf cmpf)
{
    int comp;

    while (node != NULL) {
        comp = cmpf(node, entry);
        if (comp < 0) {
            node = node->right;
        } else if (comp > 0) {
            node = node->left;
        } else {
            break;
        }
    }

    return node;
}


static struct avl_node *
_avl_node_delete(struct avl_node *node, struct avl_node *entry, avl_cmpf cmpf, struct avl_node **deleted)
{
    int comp;
    struct avl_node *succ_parent;

    if (node == NULL) {
        return NULL;
    }

    comp = cmpf(node, entry);
    if (comp < 0) {
        node->right = _avl_node_delete(node->right, entry, cmpf, deleted);
    } else if (comp > 0) {
        node->left = _avl_node_delete(node->left, entry, cmpf, deleted);
    } else {
        /*
         * Write the deleted node first
         */
        *deleted = node;

        if ((node->left == NULL) || (node->right == NULL)) {
            /*
             * 0 or 1 child
             */
            struct avl_node *child = node->left != NULL ? node->left : node->right;

            if (child == NULL) {
                node = NULL;
            } else {
                node = child;
            }
        } else {
            /*
             * 2 children
             * meaning that the successor must be in the right subtree
             */
            struct avl_node *succ = _avl_node_minp(node->right, &succ_parent);

            /*
             * Swap the nodes
             * note that after swapping, the BST property of the right subtree is preserved
             */
            if (succ_parent == node) {
                /*
                 * check special case where the successor is the right child
                 */
                node->right = succ->right;
                succ->right = node;
            } else {
                if (succ_parent->left == succ) {
                    succ_parent->left = node;
                } else {
                    succ_parent->right = node;
                }
                SWAP(&node->right, &succ->right);
            }
            SWAP(&node->left, &succ->left);
            SWAP(&node->height, &succ->height);

            /*
             * Delete the node from the right subtree
             */
            succ->right = _avl_node_delete(succ->right, node, cmpf, deleted);

            node = succ;
        }

        /* poison the node */
        _avl_node_poison(*deleted);
    }

    /*
     * balance the new head
     */
    if (node != NULL) {
        node->height = MAX(_avl_node_height(node->left), _avl_node_height(node->right)) + 1;
        node = _avl_node_balance(node);
    }

    return node;
}


struct avl_node *
avl_first(struct avl_root *root)
{
    return _avl_node_min(root->root);
}


struct avl_node *
avl_last(struct avl_root *root)
{
    return _avl_node_max(root->root);
}


struct avl_node *
avl_next(struct avl_root *root, struct avl_node *entry)
{
    struct avl_node *succ;
    struct avl_node *node;
    int comp;

    if (entry->right != NULL) {
        succ = _avl_node_min(entry->right);
    } else {
        succ = NULL;
        node = root->root;

        while (node != NULL) {
            comp = root->cmpf(node, entry);

            if (comp < 0) {
                node = node->right;
            } else if (comp > 0) {
                succ = node;
                node = node->left;
            } else {
                break;
            }
        }
    }

    return succ;
}


struct avl_node *
avl_prev(struct avl_root *root, struct avl_node *entry)
{
    struct avl_node *prev;
    struct avl_node *node;
    int comp;

    if (entry->left != NULL) {
        prev = _avl_node_max(entry->left);
    } else {
        prev = NULL;
        node = root->root;

        while (node != NULL) {
            comp = root->cmpf(node, entry);

            if (comp < 0) {
                prev = node;
                node = node->right;
            } else if (comp > 0) {
                node = node->left;
            } else {
                break;
            }
        }
    }

    return prev;
}


struct avl_node *
avl_search(struct avl_root *root, struct avl_node *entry)
{
    return _avl_node_search(root->root, entry, root->cmpf);
}


struct avl_node *
avl_insert(struct avl_root *root, struct avl_node *entry)
{
    struct avl_node *old;

    root->root = _avl_node_insert(root->root, entry, root->cmpf, &old);

    return old;
}


struct avl_node *
avl_remove(struct avl_root *root, struct avl_node *entry)
{
    struct avl_node *node;

    node = NULL;
    root->root = _avl_node_delete(root->root, entry, root->cmpf, &node);
    return node;
}


usize
avl_size(struct avl_root *root)
{
    usize size;
    struct avl_node *node;

    if (avl_empty(root))
        return 0;

    size = 0;
    node = _avl_node_min(root->root);  
    while (node != NULL) {
        size++;
        node = avl_next(root, node);
    }
    
    return size;
}

/*
 * For tests
 */

static int
_avl_node_calc_height(struct avl_node *tree)
{
    if (tree == NULL) {
        return -1;
    }

    return MAX(_avl_node_calc_height(tree->left), _avl_node_calc_height(tree->right)) + 1;
}


static int
_avl_node_test(struct avl_node *tree, avl_cmpf compare)
{
    if (tree == NULL) {
        return 1;
    }

    if (_avl_node_bf(tree) < -1 || _avl_node_bf(tree) > 1 ||
        _avl_node_calc_height(tree) != tree->height) {
        return 0;
    }

    if (tree->height == 0 && ((tree->left != NULL) || (tree->right != NULL))) {
        return 0;
    }

    if (tree->right == tree || tree->left == tree || (tree->right == tree->left && tree->right != NULL)) {
        return 0;
    }

    if ((tree->right != NULL && compare(tree, tree->right) > 0) ||
        (tree->left != NULL && compare(tree, tree->left) < 0)) {
        return 0;
    }

    return _avl_node_test(tree->left, compare) && _avl_node_test(tree->right, compare);
}

int
avl_validate(struct avl_root *root)
{
    if (avl_empty(root)) {
        return 1;
    }
    
    return _avl_node_test(root->root, root->cmpf);
}
