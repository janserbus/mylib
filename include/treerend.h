#pragma once

// Constants for visualization window dimensions and layout
#define TR_WINDOW_WIDTH 1080
#define TR_WINDOW_HEIGHT 720
#define TR_WINDOW_PADDING 50
#define TR_HEADER_HEIGHT 100

// Constants for node and font sizes
#define TR_UNIT_SIZE 60
#define TR_NODE_RADIUS TR_UNIT_SIZE / 2
#define TR_FONT_SIZE_NODE_DATA 22
#define TR_FONT_SIZE_HEADING 36

// Maximum label size for snapshots
#define TR_SNAPSHOT_MAX_LABEL_SIZE 40

// Enumeration for types of trees
// Currently only supports binary search trees
typedef enum{
    TR_TreeTypeBst,
    TR_TreeTypeAvl
}TR_TreeType;

// Structure representing a shared node structure for all trees
typedef struct _TR_SharedNode{
    int data;
    struct _TR_SharedNode *left;
    struct _TR_SharedNode *right;
    struct _TR_SharedNode *parent;
}TR_SharedNode;

// Binary search tree node structure
typedef struct _TR_BstNode{
    int data;
    struct _TR_BstNode *left;
    struct _TR_BstNode *right;
    struct _TR_BstNode *parent;
}TR_BstNode;

// Binary search AVL tree node structure
typedef struct _TR_AvlNode{
    int data;
    struct _TR_AvlNode *left;
    struct _TR_AvlNode *right;
    struct _TR_AvlNode *parent;
    int bf;
}TR_AvlNode;

// Union representing a general node for any type of tree
typedef union{
    TR_SharedNode shared;
    TR_BstNode bst;
    TR_AvlNode avl;
}TR_TreeNode;

// Structure representing a tree of a given type
typedef struct{
    TR_TreeType type;
    TR_TreeNode *root;
}TR_Tree;

// Function prototypes for library initialization and cleanup
void TR_Init(); // Initialize the library
void TR_Quit(); // Clean up resources

// Library Interface

// Function for formatting a raw tree into the library's tree structure
TR_Tree *TR_FormatTree(void *tree, TR_TreeType type);

// Presentation functions

// Render a tree visualization
void TR_RenderTree(void *tree, TR_TreeType type, char *label);
// Render a formatted tree
void TR_RenderTreeFormated(TR_Tree *tree, char * label);
// Take a snapshot of the tree visualization
void TR_TakeSnapshot(void *tree, TR_TreeType type, char *label);
// Take a snapshot of a formatted tree
void TR_TakeSnapshotFormated(TR_Tree *tree, char *label);
// Present the visualization
void TR_Present();

// Information functions

// Print snapshots of the tree
void TR_PrintSnapshots();
// Print the font path used by the library
void TR_PrintFontPath();
