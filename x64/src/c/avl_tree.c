#include "kdef.h"
#include "mm.h"
#include "avl_tree.h"

#define MAX(a, b) (((a) > (b) ? (a) : (b)))

// internal
int NATIVE64 _get_height(avl_tree_node *node)
{
    return node == NULL ? -1 : node->height;
}

int NATIVE64 _balance_factor(avl_tree_node *node)
{
    if (node == NULL)
        return 0;
    return _get_height(node->left) - _get_height(node->right);
}

avl_tree_node * NATIVE64 _right_rotate(avl_tree_node *root)
{
    avl_tree_node *left_children = root->left;
    //adjust parents first
    left_children->parent = root->parent;
    root->parent = left_children;
    if (left_children->right != NULL)
        left_children->right->parent = root;
    //perform rotation
    root->left = root->left->right;
    left_children->right = root;
    //adjust height
    root->height = MAX(_get_height(root->left), _get_height(root->right)) + 1;
    left_children->height = MAX(_get_height(left_children->left), _get_height(left_children->right)) + 1;
    return left_children;
}

avl_tree_node * NATIVE64 _left_rotate(avl_tree_node *root)
{
    avl_tree_node *right_children = root->right;
    //adjust parents
    right_children->parent = root->parent;
    root->parent = right_children;
    if (right_children->left != NULL)
        right_children->left->parent = root;
    //perform rotation
    root->right = root->right->left;
    right_children->left = root;

    root->height = MAX(_get_height(root->left), _get_height(root->right)) + 1;
    right_children->height = MAX(_get_height(right_children->left), _get_height(right_children->right)) + 1;
    return right_children;
}

avl_tree_node * NATIVE64 _balance_node(avl_tree_node *node)
{
    const int bf = _balance_factor(node);

    if (bf > 1)
    {
        const int left_bf = _balance_factor(node->left);
        if (left_bf >= 0)
            //left left
            return _right_rotate(node);
        else
        {
            //left right
            node->left = _left_rotate(node->left);
            return _right_rotate(node);
        }
    }
    else if (bf < -1)
    {
        const int right_bf = _balance_factor(node->right);
        if (right_bf <= 0)
        {
            // right right
            return _left_rotate(node);
        }
        else
        {
            // right left
            node->right = _right_rotate(node->right);
            return _left_rotate(node);
        }
    }
    else
        return node;

}


void * NATIVE64 _search(avl_tree_node *root, void *data, int(*compare)(void *, void *))
{
    if(root == NULL)
        return NULL;
    const int comp = compare(root->data, data);
    if (comp < 0)
        return _search(root->right, data, compare);
    else if (comp == 0)
        return root->data;
    else
        return _search(root->left, data, compare);
}

void * NATIVE64 avl_search(avl_tree *root, void *data, int(*compare)(void *, void *))
{
    if(root == NULL || data == NULL)
        return NULL;
    return _search(root->root,data,compare);
}

avl_tree_node * NATIVE64 _create()
{
    avl_tree_node *tree = (avl_tree_node *) (kmalloc(sizeof(avl_tree_node)));
    tree->parent = NULL;
    tree->data = NULL;
    tree->right = NULL;
    tree->left = NULL;
    tree->height = 0;
    return tree;
}

avl_tree_node * NATIVE64 _insert(avl_tree_node *root, void *data, int(*compare)(void *, void *), avl_tree_node *parent)
{
    if (data == NULL)
        return root;
    if (root == NULL)
    {
        avl_tree_node *tree = _create();
        tree->data = data;
        tree->parent = parent;
        return tree;
    }

    const int comp = compare(root->data, data);
    if (comp < 0)
    {
        root->right = _insert(root->right, data, compare, root);
    }
    else if (comp == 0)
        return root;
    else
        root->left = _insert(root->left, data, compare, root);

    root->height = MAX(_get_height(root->left), _get_height(root->right)) + 1;

    return _balance_node(root);
}

void NATIVE64 avl_insert(avl_tree *tree, void *data, int (*compare)(void *, void *))
{
    if(tree != NULL && data != NULL && compare != NULL)
    {
        if(avl_search(tree,data,compare) == NULL)
        {
            tree->root = _insert(tree->root, data, compare, NULL);
            tree->size++;
        }
    }
    return;
}

avl_tree_node * NATIVE64 _delete(avl_tree_node *root, void *data, int (*compare)(void *, void *))
{
    if (root == NULL || data == NULL)
        return root;
    const int comp = compare(root->data, data);
    if (comp < 0)
        root->right = _delete(root->right, data, compare);
    else if(comp > 0)
        root->left = _delete(root->left, data, compare);
    else
    {
        // node with only one child or no child
        if( (root->left == NULL) || (root->right == NULL) )
        {
            avl_tree_node *child = root->left != NULL ? root->left : root->right;

            if(child == NULL)
            {   // 0 child
                //free(root);
                root = NULL;
            }
            else // 1 child
            {
                //copy content of temp to root except for the parent
                root->left = child->left;
                root->right = child->right;
                root->data = child->data;
                root->height = child->height;
                //free(child);
            }
        }
        else
        {
            // node with two children: Get the inorder successor (smallest
            // in the right subtree)

            avl_tree_node * temp = root->right;
            while(temp->left != NULL)
                temp = temp->left;

            // Copy the inorder successor's data to this node
            root->data = temp->data;

            // Delete the inorder successor
            root->right = _delete(root->right, temp->data, compare);
        }
    }
    if (root == NULL)
        return root;
    root->height = MAX(_get_height(root->left), _get_height(root->right)) + 1;
    root = _balance_node(root);
    return root;
}

void NATIVE64 avl_delete(avl_tree *tree, void *data, int (*compare)(void *, void *))
{
    if(tree != NULL && data != NULL && compare != NULL && tree->size != 0)
    {
        if(avl_search(tree,data,compare) != NULL)
        {
            tree->root = _delete(tree->root, data, compare);
            tree->size--;
        }
    }
    return;
}

void NATIVE64 _free(avl_tree_node * root, void (*delete_data)(void*))
{
    if(root != NULL)
    {
        _free(root->left, delete_data);
        _free(root->right, delete_data);
        if (delete_data != NULL)
            delete_data(root->data);
    }
    return;
}

void NATIVE64 avl_free(avl_tree *tree, void (*delete_data)(void*))
{
    if(tree != NULL)
    {
        _free(tree->root, delete_data);
        kfree(tree);
    }
    return;
}

avl_tree * NATIVE64 avl_create()
{
    avl_tree* new_tree = kmalloc(sizeof(avl_tree));
    new_tree->root = NULL;
    new_tree->size = 0;
    return new_tree;
}


//iterator

avl_tree_node * NATIVE64 _smallest(avl_tree_node *root)
{
    if (root == NULL)
        return NULL;
    while (root->left != NULL)
        root = root->left;
    return root;
}

avl_tree_iterator* NATIVE64 avl_iterator(avl_tree* tree)
{
    if(tree == NULL)
        return NULL;
    avl_tree_iterator* it = (avl_tree_iterator*)kmalloc(sizeof(avl_tree_iterator));
    it->current = _smallest(tree->root);
    return it;
}

void NATIVE64 avl_next(avl_tree_iterator *it)
{
    if (it == NULL || it->current == NULL)
        return;
    avl_tree_node* root = it->current;
    if (root->right != NULL)
    {
        root = root->right;
        while (root->left != NULL)
        {
            root = root->left;
        }
        it->current = root;
        return;
    }
    else
    {
        while (root->parent != NULL)
        {
            if (root->parent->left == root)
            {
                it->current = root->parent;
                return;
            }
            root = root->parent;
        }
        it->current = NULL;
        return;
    }
}

void NATIVE64 avl_prev(avl_tree_iterator *it)
{
    if (it == NULL || it->current == NULL)
        return;
    avl_tree_node* root = it->current;
    if (root->left != NULL)
    {
        root = root->left;
        while (root->right != NULL)
        {
            root = root->right;
        }
        it->current = root;
    }
    else
    {
        while (root->parent != NULL)
        {
            if (root->parent->right == root)
            {
                it->current = root->parent;
            }
            root = root->parent;
        }
        it->current = NULL;
    }
}

// TESTS

int NATIVE64 avl_test_calculate_height(avl_tree_node * tree)
{
    if (tree == NULL)
        return -1;
    return MAX(avl_test_calculate_height(tree->left), avl_test_calculate_height(tree->right)) + 1;
}

int NATIVE64 avl_test(avl_tree_node *tree)
{
    if (tree == NULL)
        return 1;
    if (_balance_factor(tree) < -1 || _balance_factor(tree) > 1 || avl_test_calculate_height(tree) != tree->height)
        return 0;
    return avl_test(tree->left) && avl_test(tree->right);
}

