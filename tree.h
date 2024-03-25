#pragma once

typedef struct node{
    int data;
    struct node *parent;
    struct node *left;
    struct node *right;
}node;

typedef struct{
    node *root;
}type_tree;

typedef type_tree * Tree;

int node_is_list(node *n);
int node_is_root(node *n);

Tree tree();
void tree_(Tree tree);
int tree_node_count(Tree tree);
int tree_is_empty(Tree tree);
void tree_add(Tree tree, int data);
