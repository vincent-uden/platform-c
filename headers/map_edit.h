#pragma once

#include <SDL2/SDL.h>
#include <string.h>

#include "./physics.h"
#include "./rendering.h"
#include "./lifetime.h"

/*                        AABBGGRR */
#define RECT_COLOR      0xFFFFE49E
#define RECT_SHADOW     0x44000000
#define EDIT_RECT_COLOR 0xaaFFE49E
#define SEL_RECT_COLOR  0xaa68ed81

extern Lifetime lt;
extern const int SCREEN_WIDTH;
extern const int TKEYSIZE;
extern int CTRL_STATE;
extern renderLayer uiLayer;

enum tool {
    SELECT,
    RECT,
    PAN,
    TYPING_PATH,
    SAVE_CONFIRM,
    SAVE_DONE,
    LOAD_CONFIRM,
    LOAD_DONE
};

typedef struct {
    Vector position;
    Vector size;

    int selected;
} editorRect;

typedef struct rectNode rectNode;
struct rectNode {
    rectNode* next;
    editorRect value;
};
typedef rectNode* rectList;

typedef struct {
    char* path;
    rectList rl;

    Vector playerStartPos;
} mapFile;

typedef struct {
    Vector prevMousePos;
    Vector mousePos;
    Vector panStartPos;
    Vector panStartCamera;
    rectList rl;
    int cursorState;
    int gridSize;
    int tmpGridSize;
    enum tool currTool;
    enum tool prevTool;
    char* gridSizeText;

    mapFile* mf;

    SDL_Texture* cursorTexture;
    SDL_Rect* cursorRect;
    SDL_Rect pathRect;
} mapEditorState;

/* Linked list stuff */
void popRectNode(rectList list, int index);
void freeAllRectNodes(rectList list);
void printRectList(rectList list);

rectNode* addRectNode(rectList list);

int rectListLength(rectList list);

editorRect* getListRect(rectList list, int index);
/* ----------------- */

/* Map editor stuff */
int isMouseInRect(editorRect r);
int isMouseInSDL_Rect(SDL_Rect r);

mapFile loadMapFile(char* fp);

void freeMapFile(mapFile* mf);
void saveMapFile(mapFile* mf);

void mapAddPathChar(int i, mapEditorState* es);
void mapHandleInput(int* KEYS, mapEditorState* es);
void mapHandleMouseClick(int button, mapEditorState* es, worldRenderer* renderer);
void mapHandleMouseRelease(int button, mapEditorState* es, worldRenderer* renderer);
void mapEditUpdate(mapEditorState* es, worldRenderer* renderer);
void mapEditDraw(worldRenderer* renderer, mapEditorState* es);
/* ----------------- */
