#pragma once

#include <tree.h>

#define TR_WINDOW_WIDTH 1080
#define TR_WINDOW_HEIGHT 720
#define TR_WINDOW_PADDING 50
#define TR_HEADER_HEIGHT 100

#define TR_UNIT_SIZE 60
#define TR_NODE_RADIUS TR_UNIT_SIZE / 2

#define TR_FONT_SIZE_NODE_DATA 22
#define TR_FONT_SIZE_HEADING 36

#define TR_SNAPSHOT_MAX_LABEL_SIZE 40

void TR_Init();
void TR_Quit();

void TR_RenderTree(Tree tree, char *label);
void TR_Present();
void TR_TakeSnapshot(Tree tree, char *label);
void TR_PrintSnapshots();
