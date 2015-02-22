#include "type.h"
#include "kdef.h"
#include "avl_tree.h"

int64_t NATIVE64 _max(int64_t a, int64_t b)
{
    return (a > b)? a : b;
}

int64_t NATIVE64 _get_height(void * root, int64_t (*get_height)(void*))
{
    if(root == NULL)
        return -1;
    return get_height(root);
}

void *NATIVE64 _right_rotate(void *y,
        void *(*get_left)(void *),
        void (*set_left)(void *, void *),
        void *(*get_right)(void *),
        void(*set_right)(void *, void *),
        int64_t (*get_height)(void *),
        void(*set_height)(void *, int64_t))
{
    void *x = get_left(y);
    void *T2 = get_right(x);

    // Perform rotation
    set_right(x,y);
    set_left(y,T2);

    // Update heights
    set_height(y,_max(_get_height(get_left(y),get_height), _get_height(get_right(y),get_height))+1);
    set_height(x,_max(_get_height(get_left(x),get_height), _get_height(get_right(x),get_height))+1);

    // Return new root
    return x;
}

void *NATIVE64 _left_rotate(void *x,
        void *(*get_left)(void *),
        void (*set_left)(void *, void *),
        void *(*get_right)(void *),
        void(*set_right)(void *, void *),
        int64_t (*get_height)(void *),
        void(*set_height)(void *, int64_t))
{
    void *y = get_right(x);
    void *T2 = get_left(y);

    // Perform rotation
    set_left(y,x);
    set_right(x,T2);

    //  Update heights
    set_height(x, _max(_get_height(get_left(x),get_height), _get_height(get_right(x),get_height))+1);
    set_height(y, _max(_get_height(get_left(y),get_height), _get_height(get_right(y),get_height))+1);

    // Return new root
    return y;
}

// Get Balance factor of node N
int64_t NATIVE64 _get_balance_factor(void *node,
        void *(*get_left)(void *),
        void *(*get_right)(void *),
        int64_t (*get_height)(void *))
{
    if (node == NULL)
        return 0;
    return _get_height(get_left(node),get_height) - _get_height(get_right(node),get_height);
}

void*NATIVE64 avl_insert_node(void *node, void *key,
        void *(*get_left)(void *),
        void (*set_left)(void *, void *),
        void *(*get_right)(void *),
        void(*set_right)(void *, void *),
        int64_t (*get_height)(void *),
        void(*set_height)(void *, int64_t),
        int (*compare)(void *, void *))
{
    /* 1.  Perform the normal BST rotation */
    if (node == NULL)
        return key;
    if (compare(key,node) < 0)
        set_left(node, avl_insert_node(get_left(node), key, get_left, set_left, get_right, set_right, get_height, set_height, compare));
    else if (compare(key,node) == 0)
        return node;
    else
        set_right(node, avl_insert_node(get_right(node), key, get_left, set_left, get_right, set_right, get_height, set_height, compare));

    /* 2. Update height of this ancestor node */
    set_height(node, _max(_get_height(get_left(node),get_height), _get_height(get_right(node),get_height)) + 1);

    /* 3. Get the balance factor of this ancestor node to check whether
       this node became unbalanced */
    int64_t balance = _get_balance_factor(node, get_left, get_right, get_height);
    // If this node becomes unbalanced, then there are 4 cases

    // Left Left Case
    if (balance > 1 && compare(key, get_left(node)) < 0)
        return _right_rotate(node,get_left,set_left,get_right,set_right,get_height,set_height);

    // Right Right Case
    if (balance < -1 && compare(key, get_right(node)) > 0)
        return _left_rotate(node,get_left,set_left,get_right,set_right,get_height,set_height);

    // Left Right Case
    if (balance > 1 && compare(key, get_left(node)) > 0)
    {
        set_left(node, _left_rotate(get_left(node),get_left,set_left,get_right,set_right,get_height,set_height));
        return _right_rotate(node,get_left,set_left,get_right,set_right,get_height,set_height);
    }

    // Right Left Case
    if (balance < -1 && compare(key, get_right(node)) < 0)
    {
        set_right(node, _right_rotate(get_right(node),get_left,set_left,get_right,set_right,get_height,set_height));
        return _left_rotate(node,get_left,set_left,get_right,set_right,get_height,set_height);
    }

    /* return the (unchanged) node pointer */
    return node;
}

void*NATIVE64 avl_delete_node(void *root, void *key,
        void *(*get_left)(void *),
        void (*set_left)(void *, void *),
        void *(*get_right)(void *),
        void(*set_right)(void *, void *),
        int64_t (*get_height)(void *),
        void(*set_height)(void *, int64_t),
        int (*compare)(void *, void *),
        void (*set_data)(void *, void *))
{
    // STEP 1: PERFORM STANDARD BST DELETE
    if (root == NULL)
        return root;

    // If the key to be deleted is smaller than the root's key,
    // then it lies in left subtree
    if ( compare(key,root) < 0 )
        set_left(root, avl_delete_node(get_left(root), key, get_left, set_left, get_right, set_right, get_height, set_height, compare, set_data));

        // If the key to be deleted is greater than the root's key,
        // then it lies in right subtree
    else if(compare(key,root) > 0)
        set_right(root, avl_delete_node(get_right(root), key, get_left, set_left, get_right, set_right, get_height, set_height, compare, set_data));

        // if key is same as root's key, then This is the node
        // to be deleted
    else
    {
        // node with only one child or no child
        if( (get_left(root) == NULL) || (get_right(root) == NULL) )
        {
            void *temp = get_left(root) != NULL ? get_left(root) : get_right(root);

            // No child case
            if(temp == NULL)
            {
                temp = root;
                root = NULL;
            }
            else // One child case
            {
                set_height(root,_get_height(temp,get_height));
                set_left(root, get_left(temp));
                set_right(root,get_right(temp));
                set_data(root, temp);
            }

            //free(temp);
        }
        else
        {
            // node with two children: Get the inorder successor (smallest
            // in the right subtree)
            void* temp = get_right(root);
            while(get_left(temp) != NULL)
                temp = get_left(temp);

            // Copy the inorder successor's data to this node
            set_data(root, temp);

            // Delete the inorder successor
            set_right(root, avl_delete_node(get_right(root), temp, get_left, set_left, get_right, set_right, get_height, set_height, compare, set_data));
        }
    }

    // If the tree had only one node then return
    if (root == NULL)
        return root;

    // STEP 2: UPDATE HEIGHT OF THE CURRENT NODE
    set_height(root, _max(_get_height(get_left(root),get_height), _get_height(get_right(root),get_height)) + 1);

    // STEP 3: GET THE BALANCE FACTOR OF THIS NODE (to check whether
    //  this node became unbalanced)
    int64_t balance = _get_balance_factor(root,get_left,get_right,get_height);

    // If this node becomes unbalanced, then there are 4 cases

    // Left Left Case
    if (balance > 1 && _get_balance_factor(get_left(root),get_left,get_right,get_height) >= 0)
        return _right_rotate(root,get_left,set_left,get_right,set_right,get_height,set_height);

    // Left Right Case
    if (balance > 1 && _get_balance_factor(get_left(root),get_left,get_right,get_height) < 0)
    {
        set_left(root,_left_rotate(get_left(root),get_left,set_left,get_right,set_right,get_height,set_height));
        return _right_rotate(root,get_left,set_left,get_right,set_right,get_height,set_height);
    }

    // Right Right Case
    if (balance < -1 && _get_balance_factor(get_right(root),get_left,get_right,get_height) <= 0)
        return _left_rotate(root,get_left,set_left,get_right,set_right,get_height,set_height);

    // Right Left Case
    if (balance < -1 && _get_balance_factor(get_right(root),get_left,get_right,get_height) > 0)
    {
        set_right(root, _right_rotate(get_right(root),get_left,set_left,get_right,set_right,get_height,set_height));
        return _left_rotate(root,get_left,set_left,get_right,set_right,get_height,set_height);
    }

    return root;
}
