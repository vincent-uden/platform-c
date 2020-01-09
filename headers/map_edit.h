#pragma once

#include <SDL2/SDL.h>
#include <string.h>

#include "./physics.h"
#include "./rendering.h"
#include "./lifetime.h"

#define RECT_COLOR 0xFFFFE49E
#define EDIT_RECT_COLOR 0xaaFFE49E
#define SEL_RECT_COLOR 0xaa68ed81

extern Lifetime lt;
extern const int SCREEN_WIDTH;

enum tool {SELECT, RECT, TYPING_PATH};

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
    rectList rl;
    int cursorState;
    enum tool currTool;

    mapFile* mf;

    SDL_Texture* cursorTexture;
    SDL_Rect* cursorRect;
    SDL_Rect pathRect;
} mapEditorState;

void popRectNode(rectList list, int index);
void freeAllRectNodes(rectList list);
void printRectList(rectList list);

rectNode* addRectNode(rectList list);

int rectListLength(rectList list);


editorRect* getListRect(rectList list, int index);

int isMouseInRect(editorRect r);
int isMouseInSDL_Rect(SDL_Rect r);

mapFile loadMapFile(char* fp);

void freeMapFile(mapFile* mf);

void mapAddPathChar(int i, mapEditorState* es);
void mapHandleInput(int* KEYS, mapEditorState* es);
void mapHandleMouseClick(int button, mapEditorState* es);
void mapEditUpdate(mapEditorState* es);
void mapEditDraw(SDL_Renderer* renderer, mapEditorState* es);
