#include <stdlib.h>
#include <tree.h>

node *create_node(int data, node *left, node *right, node *parent)
{
    node *n = malloc(sizeof(node));

    n->data = data;
    n->left = left;
    n->right = right;
    n->parent = parent;

    return n;
}

int node_is_list(node *n)
{
    if (n->left == NULL && n->right == NULL)
    {
        return 1;
    }

    return 0;
}

int node_is_root(node *n)
{
    if (n->parent == NULL)
    {
        return 1;
    }

    return 0;
}

Tree tree()
{
    Tree t = malloc(sizeof(type_tree));
    t->root = NULL;

    return t;
}

void free_node(node *n)
{
    if (n == NULL)
    {
        return;
    }

    free_node(n->left);
    free_node(n->right);

    free(n);
}

void tree_(Tree tree)
{
    free_node(tree->root);

    free(tree);
}

int tree_is_empty(Tree tree)
{
    if (tree->root == NULL)
    {
        return 1;
    }

    return 0;
}

int count_nodes(node *root)
{
    if (root == NULL)
    {
        return 0;
    }

    int x = 0;

    x += count_nodes(root->left);
    x += count_nodes(root->right);
    x += 1;
    
    return x;
}

int tree_node_count(Tree tree)
{
    if (tree_is_empty(tree))
    {
        return 0;
    }

    return count_nodes(tree->root);
}

void tree_add(Tree tree, int data)
{
    if (tree_is_empty(tree))
    {
        tree->root = create_node(data, NULL, NULL, NULL);

        return;
    }

    node *cur = tree->root;

    while (1)
    {
        if (data < cur->data)
        {
            if (cur->left == NULL)
            {
                cur->left = create_node(data, NULL, NULL, cur);
                return;
            }

            cur = cur->left;
        }
        else
        {
            if (cur->right == NULL)
            {
                cur->right = create_node(data, NULL, NULL, cur);
                break;
            }

            cur = cur->right;
        }
    }
}