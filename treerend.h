#pragma once

#define TR_WINDOW_WIDTH 1080
#define TR_WINDOW_HEIGHT 720
#define TR_WINDOW_PADDING 50
#define TR_HEADER_HEIGHT 100

#define TR_UNIT_SIZE 60
#define TR_NODE_RADIUS TR_UNIT_SIZE / 2

#define TR_FONT_SIZE_NODE_DATA 22
#define TR_FONT_SIZE_HEADING 36

#define TR_SNAPSHOT_MAX_LABEL_SIZE 40

typedef enum{
    TR_TreeTypeBst
}TR_TreeType;

//Structure representing nessesary node structure to be rendered,
//ever other node structure has to be same exept type of pointers
typedef struct _TR_SharedNode{
    int data;
    struct _TR_SharedNode *left;
    struct _TR_SharedNode *right;
    struct _TR_SharedNode *parent;
}TR_SharedNode;

//Binary search tree node
typedef struct _TR_BstNode{
    int data;
    struct _TR_BstNode *left;
    struct _TR_BstNode *right;
    struct _TR_BstNode *parent;
}TR_BstNode;

//Structure representing general node for any type of the tree,
//operetions that are shared for every tree use shared member to acess it's members
typedef union{
    TR_SharedNode shared;
    TR_BstNode bst;
}TR_TreeNode;

//Structure representig tree of given type
typedef struct{
    TR_TreeType type;
    TR_TreeNode *root;
}TR_Tree;

void TR_Init();
void TR_Quit();

//Lib Interface

TR_Tree *TR_FormatTree(void *tree, TR_TreeType type);

//Presenting functions

void TR_RenderTree(void *tree, TR_TreeType type, char *label);
void TR_RenderTreeFormated(TR_Tree *tree, char * label);
void TR_TakeSnapshot(void *tree, TR_TreeType type, char *label);
void TR_TakeSnapshotFormated(TR_Tree *tree, char *label);
void TR_Present();

//Info functions

void TR_PrintSnapshots();
void TR_PrintFontPath();