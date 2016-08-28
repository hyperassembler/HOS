/* Copyright 2016 secXsQuared
 * Distributed under GPL license
 * See COPYING under root for details
 */

#include "k_avl_tree.h"

static inline int32_t KAPI _avl_tree_node_get_height(k_avl_tree_node_t *node)
{
    return node == NULL ? -1 : node->height;
}

static inline int32_t KAPI _avl_tree_node_get_balance_factor(k_avl_tree_node_t *node)
{
    if (node == NULL)
        return 0;
    return _avl_tree_node_get_height(node->left) - _avl_tree_node_get_height(node->right);
}

static k_avl_tree_node_t *KAPI _avl_tree_node_right_rotate(k_avl_tree_node_t *root)
{
    k_avl_tree_node_t *left_children = root->left;
    //adjust parents first
    left_children->parent = root->parent;
    root->parent = left_children;
    if (left_children->right != NULL)
        left_children->right->parent = root;
    //perform rotation
    root->left = root->left->right;
    left_children->right = root;
    //adjust height
    root->height = ke_max_32(_avl_tree_node_get_height(root->left), _avl_tree_node_get_height(root->right)) + 1;
    left_children->height =
            ke_max_32(_avl_tree_node_get_height(left_children->left), _avl_tree_node_get_height(left_children->right)) + 1;
    return left_children;
}

static k_avl_tree_node_t *KAPI _avl_tree_node_left_rotate(k_avl_tree_node_t *root)
{
    k_avl_tree_node_t *right_children = root->right;
    //adjust parents
    right_children->parent = root->parent;
    root->parent = right_children;
    if (right_children->left != NULL)
        right_children->left->parent = root;
    //perform rotation
    root->right = root->right->left;
    right_children->left = root;

    root->height = ke_max_32(_avl_tree_node_get_height(root->left), _avl_tree_node_get_height(root->right)) + 1;
    right_children->height =
            ke_max_32(_avl_tree_node_get_height(right_children->left), _avl_tree_node_get_height(right_children->right)) +
            1;
    return right_children;
}

static k_avl_tree_node_t *KAPI _avl_tree_node_balance(k_avl_tree_node_t *node)
{
    const int32_t bf = _avl_tree_node_get_balance_factor(node);

    if (bf > 1)
    {
        const int32_t left_bf = _avl_tree_node_get_balance_factor(node->left);
        if (left_bf >= 0)
            //left left
            return _avl_tree_node_right_rotate(node);
        else
        {
            //left right
            node->left = _avl_tree_node_left_rotate(node->left);
            return _avl_tree_node_right_rotate(node);
        }
    }
    else if (bf < -1)
    {
        const int32_t right_bf = _avl_tree_node_get_balance_factor(node->right);
        if (right_bf <= 0)
        {
            // right right
            return _avl_tree_node_left_rotate(node);
        }
        else
        {
            // right left
            node->right = _avl_tree_node_right_rotate(node->right);
            return _avl_tree_node_left_rotate(node);
        }
    }
    else
        return node;

}

static k_avl_tree_node_t *KAPI _avl_tree_node_insert(k_avl_tree_node_t *root, k_avl_tree_node_t *node,
                                                   k_callback_func_t compare,
                                                   k_avl_tree_node_t *parent)
{
    if (node == NULL || compare == NULL)
        return root;
    if (root == NULL)
    {
        node->parent = parent;
        return node;
    }

    const int32_t comp = compare(root, node);
    if (comp < 0)
        root->right = _avl_tree_node_insert(root->right, node, compare, root);
    else if (comp == 0)
        return root;
    else
        root->left = _avl_tree_node_insert(root->left, node, compare, root);

    root->height = ke_max_32(_avl_tree_node_get_height(root->left), _avl_tree_node_get_height(root->right)) + 1;

    return _avl_tree_node_balance(root);
}

static void _avl_tree_swap_nodes(k_avl_tree_node_t *node1, k_avl_tree_node_t *node2)
{
    if (node1 == NULL || node2 == NULL)
        return;
    k_avl_tree_node_t *parent = NULL;
    k_avl_tree_node_t *child = NULL;
    k_avl_tree_node_t *temp = NULL;
    //swap node but does not change anything else other than node1,node2
    if (node1->parent != NULL && node1->parent == node2)
    {
        parent = node2;
        child = node1;
    }
    else if (node2->parent != NULL && node2->parent == node1)
    {
        parent = node1;
        child = node2;
    }

    if (parent != NULL && child != NULL)
    {
        //connected case
        if (parent->parent != NULL)
        {
            if (parent->parent->left == parent)
                parent->parent->left = child;
            else
                parent->parent->right = child;
        }
        //update left/right for parent
        if (parent->left != NULL && child != parent->left)
        {
            parent->left->parent = child;
        }
        if (parent->right != NULL && child != parent->right)
        {
            parent->right->parent = child;
        }
        //update left/right for children
        if (child->left != NULL)
            child->left->parent = parent;
        if (child->right != NULL)
            child->right->parent = parent;

        child->parent = parent->parent;
        parent->parent = child;
        if (child == parent->left)
        {
            /*  parent
                 / \
              children */
            parent->left = child->left;
            child->left = parent;

            temp = parent->right;
            parent->right = child->right;
            child->right = temp;
        }
        else
        {
            /*  parent
                 / \
                   children */
            parent->right = child->right;
            child->right = parent;

            temp = parent->left;
            parent->left = child->left;
            child->left = temp;
        }
    }
    else
    {
        //not connected case
        //adjust all the nodes other than node1,node2
        if (node1->left != NULL)
            node1->left->parent = node2;
        if (node1->right != NULL)
            node1->right->parent = node2;

        if (node2->left != NULL)
            node2->left->parent = node1;
        if (node2->right != NULL)
            node2->right->parent = node1;

        if (node1->parent != NULL)
        {
            if (node1->parent->left == node1)
                node1->parent->left = node2;
            else
                node1->parent->right = node2;
        }

        if (node2->parent != NULL)
        {
            if (node2->parent->left == node2)
                node2->parent->left = node1;
            else
                node2->parent->right = node1;
        }

        //adjust node1,node2
        temp = node1->parent;
        node1->parent = node2->parent;
        node2->parent = temp;

        temp = node1->left;
        node1->left = node2->left;
        node2->left = temp;

        temp = node1->right;
        node1->right = node2->right;
        node2->right = temp;
    }

    //swap height
    int32_t height = node1->height;
    node1->height = node2->height;
    node2->height = height;
    return;
}

static k_avl_tree_node_t *KAPI _avl_tree_node_delete(k_avl_tree_node_t *root,
                                                   k_avl_tree_node_t *node,
                                                   k_callback_func_t compare,
                                                   k_avl_tree_node_t **deleted_node)
{
    if (root == NULL || node == NULL || compare == NULL || deleted_node == NULL)
        return root;
    const int32_t comp = compare(root, node);
    if (comp < 0)
        root->right = _avl_tree_node_delete(root->right, node, compare, deleted_node);
    else if (comp > 0)
        root->left = _avl_tree_node_delete(root->left, node, compare, deleted_node);
    else
    {
        *deleted_node = root;
        // node with only one child or no child
        if ((root->left == NULL) || (root->right == NULL))
        {
            k_avl_tree_node_t *child = root->left != NULL ? root->left : root->right;

            if (child == NULL)
            {   // 0 child
                root = NULL;
            }
            else // 1 child
            {
                child->parent = root->parent;
                root = child;
            }
        }
        else
        {
            // node with two children: Get the inorder successor (smallest
            // in the right subtree)
            k_avl_tree_node_t *successor = ke_avl_tree_larger(root);
            //swap fields
            _avl_tree_swap_nodes(successor, root);

            // Detach the inorder successor
            successor->right = _avl_tree_node_delete(successor->right, root, compare, deleted_node);

            root = successor;
        }
    }
    if (root == NULL)
        return root;
    root->height = ke_max_32(_avl_tree_node_get_height(root->left), _avl_tree_node_get_height(root->right)) + 1;
    root = _avl_tree_node_balance(root);
    return root;
}

static k_avl_tree_node_t *KAPI _avl_tree_node_search(k_avl_tree_node_t *root, k_avl_tree_node_t *node,
                                                   k_callback_func_t compare)
{
    if (root == NULL || compare == NULL)
        return NULL;
    const int32_t comp = compare(root, node);
    if (comp < 0)
        return _avl_tree_node_search(root->right, node, compare);
    else if (comp == 0)
        return root;
    else
        return _avl_tree_node_search(root->left, node, compare);
}


static void KAPI _avl_tree_node_init(k_avl_tree_node_t *it)
{
    if (it != NULL)
    {
        it->height = 0;
        it->left = NULL;
        it->right = NULL;
        it->parent = NULL;
    }
    return;
}


k_avl_tree_node_t *KAPI ke_avl_tree_smallest(k_avl_tree_t *tree)
{
    if (tree == NULL)
        return NULL;
    k_avl_tree_node_t *entry = tree->root;
    if (entry == NULL)
        return NULL;
    while (entry->left != NULL)
        entry = entry->left;
    return entry;
}

k_avl_tree_node_t *KAPI ke_avl_tree_largest(k_avl_tree_t *tree)
{
    if (tree == NULL)
        return NULL;
    k_avl_tree_node_t *entry = tree->root;
    if (entry == NULL)
        return NULL;
    while (entry->right != NULL)
        entry = entry->right;
    return entry;
}


k_avl_tree_node_t *KAPI ke_avl_tree_larger(k_avl_tree_node_t *it)
{
    if (it == NULL)
        return NULL;
    k_avl_tree_node_t *root = it;
    if (root->right != NULL)
    {
        root = root->right;
        while (root->left != NULL)
            root = root->left;
        return root;
    }
    else
    {
        while (root->parent != NULL)
        {
            if (root->parent->left == root)
                return root->parent;
            root = root->parent;
        }
        return NULL;
    }
}

k_avl_tree_node_t *KAPI ke_avl_tree_smaller(k_avl_tree_node_t *it)
{
    if (it == NULL)
        return NULL;
    k_avl_tree_node_t *root = it;
    if (root->left != NULL)
    {
        root = root->left;
        while (root->right != NULL)
            root = root->right;
        return root;
    }
    else
    {
        while (root->parent != NULL)
        {
            if (root->parent->right == root)
                return root->parent;
            root = root->parent;
        }
        return NULL;
    }
}

k_avl_tree_node_t *KAPI ke_avl_tree_search(k_avl_tree_t *tree, k_avl_tree_node_t *node)
{
    return _avl_tree_node_search(tree->root, node, tree->compare);
}


void KAPI ke_avl_tree_insert(k_avl_tree_t *tree, k_avl_tree_node_t *data)
{
    if (tree != NULL && data != NULL)
    {
        _avl_tree_node_init(data);
        tree->root = _avl_tree_node_insert(tree->root, data, tree->compare, NULL);
    }
    return;
}

k_avl_tree_node_t *KAPI ke_avl_tree_delete(k_avl_tree_t *tree, k_avl_tree_node_t *data)
{
    k_avl_tree_node_t *node = NULL;
    if (tree != NULL && data != NULL)
    {
        tree->root = _avl_tree_node_delete(tree->root, data, tree->compare, &node);
    }
    return node;
}

int32_t KAPI ke_avl_tree_size(k_avl_tree_t *tree)
{
    if (tree == NULL)
        return -1;
    if (tree->root == NULL)
        return 0;
    int32_t size = 0;
    k_avl_tree_node_t *entry = ke_avl_tree_smallest(tree);
    while (entry != NULL)
    {
        size++;
        entry = ke_avl_tree_larger(entry);
    }
    return size;
}

void KAPI ke_avl_tree_init(k_avl_tree_t *tree, k_callback_func_t compare)
{
    if (tree != NULL)
    {
        tree->compare = compare;
        tree->root = NULL;
    }
    return;
}



// TESTING STUFF

static int32_t KAPI _avl_tree_node_calculate_height(k_avl_tree_node_t *tree)
{
    if (tree == NULL)
        return -1;
    return ke_max_32(_avl_tree_node_calculate_height(tree->left), _avl_tree_node_calculate_height(tree->right)) + 1;
}

static bool KAPI _avl_tree_node_test(k_avl_tree_node_t *tree, int32_t (*compare)(k_avl_tree_node_t *, k_avl_tree_node_t *))
{
    if (tree == NULL)
        return true;
    if (_avl_tree_node_get_balance_factor(tree) < -1 || _avl_tree_node_get_balance_factor(tree) > 1 ||
        _avl_tree_node_calculate_height(tree) != tree->height)
        return false;
    if (tree->left != NULL)
    {
        if (tree->left->parent != tree)
            return false;
    }
    if (tree->right != NULL)
    {
        if (tree->right->parent != tree)
            return false;
    }
    if (compare != NULL)
    {
        if ((tree->right != NULL && compare(tree, tree->right) > 0) ||
            (tree->left != NULL && compare(tree, tree->left) < 0))
            return false;
    }
    return _avl_tree_node_test(tree->left, compare) && _avl_tree_node_test(tree->right, compare);
}

bool KAPI ke_avl_tree_validate(k_avl_tree_t *tree)
{
    if (tree == NULL)
        return true;
    return _avl_tree_node_test(tree->root, tree->compare);
}