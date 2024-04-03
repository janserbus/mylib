#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <fontconfig/fontconfig.h>

#include <list.h>
#include <treerend.h>
#include <tree.h>

//LIBRARY GLOBAL DATA STRUCTURE
typedef struct{
    SDL_Color draw_color;
    SDL_Color bg_color;
    char *font;
    List snapshots;
    int snapshots_size;
    int snapshot_pointer;
}TR_Config;

//Structure representig shared portion of every tree data 
//(meaning regardles of the data type, every data will have this exact structure)
typedef struct{
    int data;
    int x;
    int y;
    int parent;
}TR_SharedData;

typedef struct{
    int data;
    int x;
    int y;
    int parent;
}TR_BstData;

//Shared member is used to do operations that are shared among all the data types
typedef union{
    TR_SharedData shared;
    TR_BstData bst;
}TR_NodeData;

//Dual structure to TR_Tree representig tree ready to render
typedef struct{
    char *label;
    TR_TreeType type;
    List node_data;
}TR_Snapshot;

//SNAPSHOT
TR_Snapshot *TR_CreateSnapshot(TR_Tree *tree, char *label);
void TR_DestroySnapshot(TR_Snapshot *snap);
void TR_PrintSnapshot(TR_Snapshot *snap);

//NODE DATA
TR_NodeData *TR_CreateNodeData();
void TR_DestroyNodeData(TR_NodeData *data);
void TR_ConvertToNodeData(TR_Tree *tree, List node_data);
int TR_ConvertNodes(TR_TreeNode *root, TR_TreeType type, int x, int y, int left_child, List data);

//DRAWING
void TR_RenderNextSnapshot(SDL_Renderer *rend);
void TR_RenderPrevSnapshot(SDL_Renderer *rend);
void TR_RenderDrawSnapshot(SDL_Renderer *rend, TR_Snapshot *snap);
void TR_RenderDrawNodeData(SDL_Renderer *rend, List node_data, SDL_Rect rend_area);
void TR_RenderDrawNode(SDL_Renderer *rend, SDL_Point coords, int data, float sclf);
void TR_RenderDrawConnection(SDL_Renderer *rend, SDL_Point c1, SDL_Point c2, float sclf);

//ANALYTIC GEOMETRY
SDL_Point RectCenter(SDL_Rect rect);
SDL_Point Vector(SDL_Point p1, SDL_Point p2);
SDL_Point VectorMultiply(SDL_Point vec, float fac);
double VectorAngle(SDL_Point vec);

//FONT LOADING
char *TR_GetFontPath(char *family);

//ERROR HANDELING
void TR_ErrorLog(char *function, const char * msg);

//Library global data
TR_Config Data;

//Initialization of TreeRend module
void TR_Init()
{
    char *err_fun_name = "TR_Init";

    //Initialize libraries
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_EVENTS) != 0)
    {
        TR_ErrorLog(err_fun_name, SDL_GetError());
        goto exit;
    }

    if(TTF_Init() != 0)
    {
        TR_ErrorLog(err_fun_name, TTF_GetError());
        goto destr_SDL;
    }

    //sets render scale quality
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

    SDL_Color d = {230, 230, 230, 255};
    SDL_Color b = {15, 15, 15, 255};

    Data.draw_color = d;
    Data.bg_color = b;
    Data.font = TR_GetFontPath("Roboto");
    Data.snapshots = list();
    if (Data.snapshots == NULL)
    {
        TR_ErrorLog(err_fun_name, "Unable to initialize list");
        goto destr_TTF;
    }
    Data.snapshots_size = 0;
    Data.snapshot_pointer = 1;

    return;

    destr_TTF:
    TTF_Quit();

    destr_SDL:
    SDL_Quit();

    exit:
    exit(EXIT_FAILURE);
}

void TR_Quit()
{
    list_(Data.snapshots, &TR_DestroySnapshot);
    TTF_Quit();
    SDL_QuitSubSystem(SDL_INIT_VIDEO|SDL_INIT_EVENTS);
    SDL_Quit();
}

/////////////////////////////////////////////////////////////////////
///////////              INTERFACE FUNCTIONS

TR_Tree *TR_FormatTree(void *tree, TR_TreeType type)
{
    TR_Tree *t = malloc(sizeof(TR_Tree));

    t->type = type;
    t->root = *((void **) tree);

    return t;
}

/////////////////////////////////////////////////////////////////////
///////////         TREEREND PRESENTING FUNCTIONS


void TR_RenderTree(void *tree, TR_TreeType type, char *label)
{
    TR_Tree *formated = TR_FormatTree(tree, type);

    TR_RenderTreeFormated(formated, label);

    free(formated);
}

void TR_RenderTreeFormated(TR_Tree *tree, char * label)
{
    char *err_fun_name = "TR_RenderTreeFormated";

    //Setup window and renderer
    SDL_Window *window = SDL_CreateWindow("TreeRend", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, TR_WINDOW_WIDTH, TR_WINDOW_HEIGHT, SDL_WINDOW_SHOWN);

    if(!window)
    {
        TR_ErrorLog(err_fun_name, SDL_GetError());
        goto exit;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, 0, SDL_RENDERER_ACCELERATED);

    if(!renderer)
    {
        TR_ErrorLog(err_fun_name, SDL_GetError());
        goto destr_window;
    }

    //Prepare renderer
    SDL_SetRenderDrawColor(renderer, Data.bg_color.r, Data.bg_color.g, Data.bg_color.b, Data.bg_color.a);

    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, Data.draw_color.r, Data.draw_color.g, Data.draw_color.b, Data.draw_color.a);

    //Take snapshot
    TR_Snapshot *snap = TR_CreateSnapshot(tree, label);

    //Draw snapshot into renderer
    TR_RenderDrawSnapshot(renderer, snap);    

    SDL_RenderPresent(renderer);

    //reset renderer
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    //waits until window is closed
    int active = 1;
    while (active)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                active = 0;
            }
        }        
    }

    TR_DestroySnapshot(snap);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    return;

    destr_window:
    SDL_DestroyWindow(window);

    exit:
    TR_Quit();
    exit(EXIT_FAILURE);

}

void TR_Present()
{
    if (list_is_empty(Data.snapshots))
    {
        return;
    }

    //Setup window and renderer
    SDL_Window *window = SDL_CreateWindow("TreeRend", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, TR_WINDOW_WIDTH, TR_WINDOW_HEIGHT, SDL_WINDOW_SHOWN);

    if(!window)
    {
        perror(SDL_GetError());
        TR_Quit();
        abort();
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, 0, SDL_RENDERER_ACCELERATED);

    if(!renderer)
    {
        perror(SDL_GetError());
        SDL_DestroyWindow(window);
        TR_Quit();
        abort();
    }

    TR_RenderNextSnapshot(renderer);

    //waits until window is closed
    int active = 1;
    while (active)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_QUIT:
                    active = 0;
                break;

                case SDL_KEYDOWN:

                    if(event.key.keysym.sym == SDLK_RIGHT)
                    {
                        TR_RenderNextSnapshot(renderer);
                    }
                    else if(event.key.keysym.sym == SDLK_LEFT)
                    {
                        TR_RenderPrevSnapshot(renderer);
                    }

                break;                
            }
        }        
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}

//Renders next snapshot in the list (snapshot pointed to by Data.snapshot_pointer) and moves pointer acordingly
void TR_RenderNextSnapshot(SDL_Renderer *rend)
{
    if (Data.snapshot_pointer > Data.snapshots_size)
    {
        return;
    }

    //Prepare renderer
    SDL_SetRenderDrawColor(rend, Data.bg_color.r, Data.bg_color.g, Data.bg_color.b, Data.bg_color.a);

    SDL_RenderClear(rend);

    SDL_SetRenderDrawColor(rend, Data.draw_color.r, Data.draw_color.g, Data.draw_color.b, Data.draw_color.a);

    //Get snapshot
    TR_Snapshot *snap = list_get_nth(Data.snapshots, Data.snapshot_pointer);
    Data.snapshot_pointer++;

    //Draw snapshot into renderer
    TR_RenderDrawSnapshot(rend, snap);

    SDL_RenderPresent(rend);
}

void TR_RenderPrevSnapshot(SDL_Renderer *rend)
{
    if (Data.snapshot_pointer - 2 < 1)
    {
        return;
    }

    //Prepare renderer
    SDL_SetRenderDrawColor(rend, Data.bg_color.r, Data.bg_color.g, Data.bg_color.b, Data.bg_color.a);

    SDL_RenderClear(rend);

    SDL_SetRenderDrawColor(rend, Data.draw_color.r, Data.draw_color.g, Data.draw_color.b, Data.draw_color.a);

    //Get snapshot
    TR_Snapshot *snap = list_get_nth(Data.snapshots, Data.snapshot_pointer - 2);
    Data.snapshot_pointer--;

    //Draw snapshot into renderer
    TR_RenderDrawSnapshot(rend, snap);    

    SDL_RenderPresent(rend);
}

/////////////////////////////////////////////////////////////////////
///////////         SNAPSHOT FUNCTIONS

void TR_TakeSnapshot(void *tree, TR_TreeType type, char *label)
{
    TR_Tree *formated = TR_FormatTree(tree, type);

    TR_TakeSnapshotFormated(formated, label);

    free(formated);
}

void TR_TakeSnapshotFormated(TR_Tree *tree, char *label)
{
    //Create snapshot
    TR_Snapshot *snap = TR_CreateSnapshot(tree, label);

    //Store snapshot
    list_add_end(Data.snapshots, snap);
    Data.snapshots_size++;
}

TR_Snapshot *TR_CreateSnapshot(TR_Tree *tree, char *label)
{
    TR_Snapshot *snap = malloc(sizeof(TR_Snapshot));

    if(label != NULL)
    {
        snap->label = malloc((TR_SNAPSHOT_MAX_LABEL_SIZE + 1) * sizeof(char));

        strcpy(snap->label, label);
    }
    else
    {
        snap->label = NULL;
    }

    snap->type = tree->type;
    snap->node_data = list();

    TR_ConvertToNodeData(tree, snap->node_data);

    return snap;
}

void TR_DestroySnapshot(TR_Snapshot *snap)
{
    list_(snap->node_data, &TR_DestroyNodeData);

    if (snap->label != NULL)
    {
        free(snap->label);
    }
    
    free(snap);
}

void TR_PrintSnapshot(TR_Snapshot *snap)
{
    if (snap->label != NULL)
    {
        printf("\n%s\n-------------\n", snap->label);
    }

    void *el;
        switch (snap->type)
        {
            case TR_TreeTypeBst:
                int i = 1;
                while (list_foreach(snap->node_data, &el))
                {
                    TR_NodeData *data = (TR_NodeData *) el;

                    printf("element: %i\n", i);
                    printf("  data: %i\n", data->bst.data);
                    printf("     x: %i\n", data->bst.x);
                    printf("     y: %i\n", data->bst.y);
                    printf("parent: %i\n\n", data->bst.parent);
                    i++;
                }
                break;

            default:
                printf("Unknown type\n\n");
                break;
        }
}

void TR_PrintSnapshots()
{
    list_print(Data.snapshots, &TR_PrintSnapshot);
}


/////////////////////////////////////////////////////////////////////
///////////         NODEDATDA FUNCTIONS

TR_NodeData *TR_CreateNodeData()
{
    return malloc(sizeof(TR_NodeData));
}

void TR_DestroyNodeData(TR_NodeData *data)
{
    free(data);
}

int TR_CompNodeData(TR_NodeData *d1, TR_NodeData *d2)
{
    if (d1->shared.x > d2->shared.x)
    {
        return 1;
    }
    else if(d1->shared.x < d2->shared.x)
    {
        return -1;
    }
    
    return 0;
}

void TR_ConvertToNodeData(TR_Tree *tree, List node_data)
{
    TR_ConvertNodes(tree->root, tree->type, 0, 0, -1, node_data);

    list_sort(node_data, &TR_CompNodeData);
}

//y is y coordinate of curent node
//returns curent known most 
int TR_ConvertNodes(TR_TreeNode *root, TR_TreeType type, int x, int y, int left_child, List node_data)
{
    TR_NodeData *d = TR_CreateNodeData();

    d->shared.data = root->shared.data;
    d->shared.y = y;

    int most_right = x;
    int ret = x;

    if (root->shared.left != NULL) 
    {
        ret = TR_ConvertNodes((TR_TreeNode *) root->shared.left, type, x, y + 1, 1, node_data);
    }

    ret += 1;

    d->shared.x = ret;

    if (root->shared.right != NULL)
    {
        ret = TR_ConvertNodes((TR_TreeNode *) root->shared.right, type, ret, y + 1, 0, node_data);
    }

    if (root->shared.parent == NULL)
    {
        d->shared.parent = -1;
    }
    else if(left_child)
    {
        d->shared.parent = ret + 1;
    }
    else 
    {
        d->shared.parent = most_right;
    }

    list_add(node_data, d);

    return ret;    
}

/////////////////////////////////////////////////////////////////////
///////////         RENDERIGN FUNCTIONS

void TR_RenderDrawSnapshot(SDL_Renderer *rend, TR_Snapshot *snap)
{
    if (snap->label != NULL)
    {
        TTF_Font *font = TTF_OpenFont(Data.font, TR_FONT_SIZE_HEADING);

        if (font == NULL)
        {
            perror(TTF_GetError());
            TR_Quit();
            abort();
        }

        SDL_Surface *label = TTF_RenderText_Blended(font, snap->label, Data.draw_color);

        if (label == NULL)
        {
            perror(SDL_GetError());
            TTF_CloseFont(font);
            TR_Quit();
            abort();
        }

        SDL_Texture *tex = SDL_CreateTextureFromSurface(rend, label);

        if (tex == NULL)
        {
            perror(SDL_GetError());
            SDL_FreeSurface(label);
            TTF_CloseFont(font);
            TR_Quit();
            abort();
        }

        SDL_Rect rect = {
            TR_WINDOW_WIDTH / 2 - label->w / 2,
            TR_HEADER_HEIGHT / 2 - label->h / 2,
            label->w,
            label->h
        };

        SDL_RenderCopy(rend, tex, NULL, &rect);

        SDL_DestroyTexture(tex);
        SDL_FreeSurface(label);

        SDL_Rect area ={
            TR_WINDOW_PADDING,
            TR_HEADER_HEIGHT,
            TR_WINDOW_WIDTH - 2 * TR_WINDOW_PADDING,
            TR_WINDOW_HEIGHT - TR_HEADER_HEIGHT - TR_WINDOW_PADDING
        };

        TR_RenderDrawNodeData(rend, snap->node_data, area);

        TTF_CloseFont(font);
    }
    else
    {
        SDL_Rect area ={
            TR_WINDOW_PADDING,
            TR_WINDOW_PADDING,
            TR_WINDOW_WIDTH - 2 * TR_WINDOW_PADDING,
            TR_WINDOW_HEIGHT - 2 * TR_WINDOW_PADDING
        };

        TR_RenderDrawNodeData(rend, snap->node_data, area);
    }
}

void TR_RenderDrawNodeData(SDL_Renderer *rend, List node_data, SDL_Rect rend_area)
{
    //Transform coordinates
    int lowest = 0;
    int most_right = 0;

    // Projit seznam a najit nejpravejsi a nejnizsi prve a bypocita rozmer stromu
    void *el;
    while (list_foreach(node_data, &el))
    {
        TR_NodeData *data = (TR_NodeData *) el;

        if (data->shared.y > lowest)
        {
            lowest = data->shared.y;
        }

        if (data->shared.x > most_right)
        {
            most_right = data->shared.x;
        }
    }

    int corner = 1 * TR_UNIT_SIZE - TR_NODE_RADIUS;
    int width = (most_right - 1) * TR_UNIT_SIZE + 2 * TR_NODE_RADIUS;
    int height = (lowest - 1) * TR_UNIT_SIZE + 2 * TR_NODE_RADIUS;

    //Tree screen area
    SDL_Rect tree_area = {
        corner,
        corner,
        width,
        height
    };

    //Calculate move vector
    SDL_Point ta_center = RectCenter(tree_area);
    SDL_Point ra_center = RectCenter(rend_area);

    //Move vector
    SDL_Point movv = Vector(ta_center, ra_center);

    //Calculate scale factor
    float sclf = 1;

    //Picks the longer side of tree_area and calulates sclf to fit that side into rend_area
    if (tree_area.w > tree_area.h)
    {
        sclf = rend_area.w / (float) tree_area.w;
    }
    else
    {
        sclf = rend_area.h / (float) tree_area.h;
    }

    //Ensures that scaling only happens down (for sclf < 1)
    if (sclf >= 1)
    {
        sclf = 1;
    }

    //Draw nodes
    while (list_foreach(node_data, &el))
    {
        TR_NodeData *data = (TR_NodeData *) el;

        SDL_Point data_unit_coords = {data->shared.x * TR_UNIT_SIZE, data->shared.y * TR_UNIT_SIZE};

        SDL_Point vec = Vector(data_unit_coords, ta_center);

        //We calculate scale vector for every individual node
        SDL_Point sclv = VectorMultiply(vec, 1 - sclf);

        //Calculates transformed coordinates
        SDL_Point data_screen_coords = {
            data_unit_coords.x + movv.x + sclv.x,
            data_unit_coords.y + movv.y + sclv.y
        };

        TR_RenderDrawNode(rend, data_screen_coords, data->shared.data, sclf);

        //If node isn't root render int's connection
        if (data->shared.parent >= 0)
        {
            //We calculate scale vector for every individual node
            TR_NodeData *parent = list_get_nth(node_data, data->shared.parent);

            SDL_Point parent_unit_coords = {parent->shared.x * TR_UNIT_SIZE, parent->shared.y * TR_UNIT_SIZE};

            sclv = VectorMultiply(Vector(parent_unit_coords, ta_center), 1 - sclf);

            SDL_Point parent_screen_coords = {
                parent_unit_coords.x + movv.x + sclv.x,
                parent_unit_coords.y + movv.y + sclv.y
            };

            TR_RenderDrawConnection(rend, data_screen_coords, parent_screen_coords, sclf);
        }
    }
}

void TR_RenderDrawNode(SDL_Renderer *rend, SDL_Point coords, int data, float sclf)
{
    TTF_Font *font = TTF_OpenFont(Data.font, TR_FONT_SIZE_NODE_DATA * sclf);

    if (font == NULL)
    {
        perror(TTF_GetError());
        TR_Quit();
        abort();
    }

    char *buffer = malloc(TR_SNAPSHOT_MAX_LABEL_SIZE * sizeof(char));

    sprintf(buffer, "%i", data);

    SDL_Surface *num = TTF_RenderText_Blended(font, buffer, Data.draw_color);

    if (num == NULL)
    {
        perror(SDL_GetError());
        free(buffer);
        TTF_CloseFont(font);
        TR_Quit();
        abort();
    }

    free(buffer);

    SDL_Texture *tex = SDL_CreateTextureFromSurface(rend, num);

    if (tex == NULL)
    {
        perror(SDL_GetError());
        SDL_FreeSurface(num);
        TTF_CloseFont(font);
        TR_Quit();
        abort();
    }

    SDL_Rect num_rect = {
        coords.x - num->w / 2,
        coords.y - num->h / 2,
        num->w,
        num->h        
    };

    SDL_RenderCopy(rend, tex, NULL, &num_rect);

    aacircleColor(rend, coords.x, coords.y, TR_NODE_RADIUS * sclf, *((Uint32 *) &Data.draw_color.r));

    TTF_CloseFont(font);
    SDL_DestroyTexture(tex);
    SDL_FreeSurface(num);
}

void TR_RenderDrawConnection(SDL_Renderer *rend, SDL_Point c1, SDL_Point c2, float sclf)
{
    //calucate directional vector from c1 to c2
    SDL_Point dir = Vector(c1, c2);

    //calculate angle of the dir vector
    double alfa = VectorAngle(dir);

    //calculate unit coordinates of the circle and connecting segment intersection
    int intersec_x = round(TR_NODE_RADIUS * sclf * cos(alfa));
    int intersec_y = round(TR_NODE_RADIUS * sclf * sin(alfa));

    SDL_Point p1 = {
        c1.x + intersec_x,
        c1.y + intersec_y
    };

    SDL_Point p2 = {
        c2.x - intersec_x,
        c2.y - intersec_y
    };
    
    //Connect two intersecting points
    aalineColor(rend, p1.x, p1.y, p2.x, p2.y, *((Uint32 *) &Data.draw_color.r));
}

/////////////////////////////////////////////////////////////////////
///////////     ANALYTIC GEOMETRY FUNCTIONS

SDL_Point RectCenter(SDL_Rect rect)
{
    SDL_Point p = {
        rect.x + rect.w / 2,
        rect.y + rect.h / 2
    };

    return p;
}

SDL_Point VectorMultiply(SDL_Point vec, float fac)
{
    SDL_Point v = {
        vec.x * fac,
        vec.y * fac
    };

    return v;
}

SDL_Point Vector(SDL_Point p1, SDL_Point p2)
{    
    SDL_Point vec = {
        p2.x - p1.x,
        p2.y - p1.y
    };

    return vec;
}

double VectorAngle(SDL_Point vec)
{
    const double PI = 3.14159265358979323846;

    if (vec.x >= 0 && vec.y >= 0)
    {
        return atan(vec.y / (double) vec.x);
    }
    else if (vec.x <= 0 && vec.y >= 0)
    {
        return PI - atan(vec.y / (double) -1 * vec.x);
    }
    else if (vec.x <= 0 && vec.y <= 0)
    {
        return PI + atan(vec.y / (double) vec.x);
    }
    else
    {
        return 2 * PI - atan(-1 * vec.y / (double) vec.x);
    }
}

/////////////////////////////////////////////////////////////////////
///////////             FONT LOADING FUNCTIONS

char *TR_GetFontPath(char *family)
{
    char *err_fun_name = "TR_GetFontPath";

    //Initialize Fontconfig
    FcConfig *config = FcInitLoadConfigAndFonts();

    if (config == NULL)
    {
        TR_ErrorLog(err_fun_name, "Unable to load Font configuration");
        goto exit;
    }

    //Creat Fontpattern
    FcPattern *pattern = FcPatternCreate();

    if (pattern == NULL)
    {
        TR_ErrorLog(err_fun_name, "Unable to create Font patern");
        goto config_destr;
    }

    FcPatternAddString(pattern, FC_FAMILY, (FcChar8 *) family);
    FcPatternAddInteger(pattern, FC_WEIGHT, FC_WEIGHT_NORMAL);
    FcPatternAddInteger(pattern, FC_SLANT, FC_SLANT_ROMAN);

    FcConfigSubstitute(config, pattern, FcMatchPattern);
    FcDefaultSubstitute(pattern);


    //Match font
    FcResult result;
    FcPattern *font = FcFontMatch(config, pattern, &result);

    if (font == NULL)
    {
        TR_ErrorLog(err_fun_name, "Font not found");
        goto pattern_destr;
    }

    char *path;

    FcResult r = FcPatternGetString(font, FC_FILE, 0, (FcChar8 **) &path);

    if(r != FcResultMatch)
    {
        TR_ErrorLog(err_fun_name, "Unable to get font file path");
        goto font_destr;
    }

    int pathlen = strlen(path);

    char *ret = malloc((pathlen + 1) * sizeof(char));

    strcpy(ret, path); 

    FcPatternDestroy(font);
    FcPatternDestroy(pattern);
    FcConfigDestroy(config);

    return ret;

    //In case of error
    font_destr:
    FcPatternDestroy(font);

    pattern_destr:
    FcPatternDestroy(pattern);

    config_destr:
    FcConfigDestroy(config);

    exit:
    TR_Quit();
    exit(EXIT_FAILURE);
}

void TR_PrintFontPath()
{
    printf("%s\n", Data.font);
}

/////////////////////////////////////////////////////////////////////
///////////             ERROR HANDELING

void TR_ErrorLog(char *function, const char * msg)
{
    printf("[TreeRend Error] - %s: %s\n", function, msg);
}