#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <math.h>

#include <list.h>
#include <treerend.h>

#define PI 3.14159265358979323846

//LIBRARY GLOBAL DATA STRUCTURE
typedef struct{
    SDL_Color draw_color;
    SDL_Color bg_color;
    char *font;
    List snapshots;
    int snapshots_size;
    int snapshot_pointer;
}TR_Data;

typedef struct{
    int data;
    int x;
    int y;
    int parent;
}TR_TreeData;

typedef struct{
    char *label;
    TR_TreeData *data;
    int data_size;
}TR_Snapshot;

//SNAPSHOT
TR_Snapshot *TR_CreateSnapshot(Tree tree, char *label);
void TR_DestroySnapshot(TR_Snapshot *snap);

//TREE DATA
TR_TreeData *TR_ConvertToTreeData(Tree tree, int data_size);
int TR_TreeConvert(node *r, int x, int y, int left_child, TR_TreeData* data);
void TR_DestroyTreeData(TR_TreeData *data);
void TR_PrintTreeData(TR_TreeData *data, int size);

void TR_RenderNextSnapshot(SDL_Renderer *rend);
void TR_RenderPrevSnapshot(SDL_Renderer *rend);

//DRAWING
void TR_RenderDrawSnapshot(SDL_Renderer *rend, TR_Snapshot *snap);
void TR_RenderDrawTreeData(SDL_Renderer *rend, TR_TreeData *data, int size, SDL_Rect rend_area);
void TR_RenderDrawNode(SDL_Renderer *rend, SDL_Point coords, int data, float sclf);
void TR_RenderDrawConnection(SDL_Renderer *rend, SDL_Point c1, SDL_Point c2, float sclf);
void TR_RenderDrawCircle(SDL_Renderer *rend, SDL_Point center, int radius);

//ANALYTIC GEOMETRY
SDL_Point RectCenter(SDL_Rect rect);
SDL_Point Vector(SDL_Point p1, SDL_Point p2);
SDL_Point VectorMultiply(SDL_Point vec, float fac);
double VectorAngle(SDL_Point vec);


//Library global data
TR_Data Data;

//Initialization of TreeRend module
void TR_Init()
{
    //initializes libraries
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_EVENTS) != 0)
    {
        perror(SDL_GetError());
        abort();    
    }

    if(TTF_Init() != 0){
        perror(TTF_GetError());
        SDL_Quit();
        abort();
    }

    //sets render scale quality
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

    SDL_Color d = {230, 230, 230, 255};
    SDL_Color b = {15, 15, 15, 255};

    Data.draw_color = d;
    Data.bg_color = b;
    Data.font = "/home/jan/Documents/inf23_24/mylib/treerend/Roboto.ttf";
    Data.snapshots = list();
    Data.snapshots_size = 0;
    Data.snapshot_pointer = 1;
}

void TR_Quit()
{
    list_(Data.snapshots, &TR_DestroySnapshot);
    TTF_Quit();
    SDL_QuitSubSystem(SDL_INIT_VIDEO|SDL_INIT_EVENTS);
    SDL_Quit();
}

/////////////////////////////////////////////////////////////////////
///////////         TREEREND PRESENTING FUNCTIONS

void TR_RenderTree(Tree tree, char *label)
{
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

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
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

void TR_TakeSnapshot(Tree tree, char *label)
{
    //Create snapshot
    TR_Snapshot *snap = TR_CreateSnapshot(tree, label);

    //Store snapshot
    list_add_end(Data.snapshots, snap);
    Data.snapshots_size++;
}

TR_Snapshot *TR_CreateSnapshot(Tree tree, char *label)
{
    TR_Snapshot *snap = malloc(sizeof(TR_Snapshot));

    if(label != NULL)
    {
        snap->label = malloc(TR_SNAPSHOT_MAX_LABEL_SIZE * sizeof(char));

        strcpy(snap->label, label);
    }
    else
    {
        snap->label = NULL;
    }

    snap->data_size = tree_node_count(tree);
    snap->data = TR_ConvertToTreeData(tree, snap->data_size);

    return snap;
}

void TR_DestroySnapshot(TR_Snapshot *snap)
{
    TR_DestroyTreeData(snap->data);
    if (snap->label != NULL)
    {
        free(snap->label);
    }
    free(snap);
}

void TR_PrintSnapshots()
{
    for (int i = 1; i <= Data.snapshots_size; i++)
    {
        printf("\n\n");

        TR_Snapshot *snap = list_get_nth(Data.snapshots, i);
        
        printf("%s\n\n", snap->label);

        TR_PrintTreeData(snap->data, snap->data_size);

    }

    printf("\n\n\n");
}


/////////////////////////////////////////////////////////////////////
///////////         TREEDATA FUNCTIONS

TR_TreeData *TR_ConvertToTreeData(Tree tree, int data_size)
{
    //allocs array for TR_TreeData
    TR_TreeData *data = calloc(data_size, sizeof(TR_TreeData));

    TR_TreeConvert(tree->root, 0, 1, -1, data);

    return data;
}

int TR_TreeConvert(node *r, int x, int y, int left_child, TR_TreeData* data)
{
    TR_TreeData d = {0, 0, 0, 0};

    d.data = r->data;
    d.y = y;

    int most_right = x;
    int ret = x;

    if (r->left != NULL) 
    {
        ret = TR_TreeConvert(r->left, x, y + 1, 1, data);
    }

    ret += 1;

    d.x = ret;

    if (r->right != NULL)
    {
        ret = TR_TreeConvert(r->right, ret, y + 1, 0, data);
    }

    if (node_is_root(r))
    {
        d.parent = -1;
    }
    else if(left_child)
    {
        d.parent = ret + 1;
    }
    else 
    {
        d.parent = most_right;
    }

    data[d.x - 1] = d;

    return ret;    
}

void TR_DestroyTreeData(TR_TreeData *data)
{
    free(data);
}

void TR_PrintTreeData(TR_TreeData *data, int size)
{
    for (int  i = 0; i < size; i++)
    {
        printf("element: %i\n", i);
        printf("  data: %i\n", data[i].data);
        printf("     x: %i\n", data[i].x);
        printf("     y: %i\n", data[i].y);
        printf("parent: %i\n\n", data[i].parent);
    }
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

        TR_RenderDrawTreeData(rend, snap->data, snap->data_size, area);

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

        SDL_RenderDrawRect(rend, &area);

        TR_RenderDrawTreeData(rend, snap->data, snap->data_size, area);
    }

}

void TR_RenderDrawTreeData(SDL_Renderer *rend, TR_TreeData *data, int size, SDL_Rect rend_area)
{
    //Transform coordinates
    int lowest = 0;

    for (int i = 0; i < size; i++)
    {
        if (data[i].y > lowest)
        {
            lowest = data[i].y;
        }
    }

    int corner = 1 * TR_UNIT_SIZE - TR_NODE_RADIUS;
    int width = (size - 1) * TR_UNIT_SIZE + 2 * TR_NODE_RADIUS;
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
    for (int i = 0; i < size; i++)
    {
        TR_TreeData d = data[i];

        SDL_Point d_unit_coords = {d.x * TR_UNIT_SIZE, d.y * TR_UNIT_SIZE};

        SDL_Point vec = Vector(d_unit_coords, ta_center);

        //We calculate scale vector for every individual node
        SDL_Point sclv = VectorMultiply(vec, 1 - sclf);

        //Calculates transformed coordinates
        SDL_Point d_screen_coords = {
            d_unit_coords.x + movv.x + sclv.x,
            d_unit_coords.y + movv.y + sclv.y
        };

        TR_RenderDrawNode(rend, d_screen_coords, d.data, sclf);

        //If node isn't root render int's connection
        if (data[i].parent >= 0)
        {
            //We calculate scale vector for every individual node
            TR_TreeData p = data[data[i].parent - 1];

            SDL_Point p_unit_coords = {p.x * TR_UNIT_SIZE, p.y * TR_UNIT_SIZE};

            sclv = VectorMultiply(Vector(p_unit_coords, ta_center), 1 - sclf);

            SDL_Point p_screen_coords = {
                p_unit_coords.x + movv.x + sclv.x,
                p_unit_coords.y + movv.y + sclv.y
            };

            TR_RenderDrawConnection(rend, d_screen_coords, p_screen_coords, sclf);
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

    TR_RenderDrawCircle(rend, coords, TR_NODE_RADIUS * sclf);

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
    SDL_RenderDrawLine(rend, p1.x, p1.y, p2.x, p2.y);
}

void TR_RenderDrawCircle(SDL_Renderer *rend, SDL_Point center, int radius)
{
    //Implementing midpoint algorithm

    int diameter = 2 * radius;

    int x = radius - 1;
    int y = 0;
    int tx = 1;
    int ty = 1;
    int error = tx - diameter;

    while (x >= y)
    {        
        //sEach of the following renders an octant of the circle
        SDL_RenderDrawPoint(rend, center.x + x, center.y - y);
        SDL_RenderDrawPoint(rend, center.x + x, center.y + y);
        SDL_RenderDrawPoint(rend, center.x - x, center.y - y);
        SDL_RenderDrawPoint(rend, center.x - x, center.y + y);
        SDL_RenderDrawPoint(rend, center.x + y, center.y - x);
        SDL_RenderDrawPoint(rend, center.x + y, center.y + x);
        SDL_RenderDrawPoint(rend, center.x - y, center.y - x);
        SDL_RenderDrawPoint(rend, center.x - y, center.y + x);

        if (error <= 0)
        {
            y += 1;
            error += ty;
            ty += 2;
        }

        if (error > 0)
        {
            x -= 1;
            tx += 2;
            error += (tx - diameter);
        }
    }     
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