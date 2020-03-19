#include "../headers/map_edit.h"

/* Deallocates the memory for the removed node */
void popRectNode(rectList list, int index) {
    rectNode* curr = list;
    for ( int i = 0; i < index - 1; i++ ) {
        curr = curr->next;
    }

    rectNode* toBeRemoved = curr->next;
    curr->next = toBeRemoved->next;
    printf("%p\n", toBeRemoved->next);
    POP_LT_PTR(lt, toBeRemoved);
}

/* Deallocates the memory for all nodes in the stack */
void freeAllRectNodes(rectList list) {
    int length = rectListLength(list);
    rectNode** ptrs = malloc(length * sizeof(rectNode*));
    PUSH_LT(lt, ptrs, free);
    rectNode* curr = list;
    rectNode* next;
    while ( curr != NULL ) {
        next = curr->next;
        POP_LT_PTR(lt, curr);
        curr = next;
    }
    POP_LT_PTR(lt, ptrs);
}

/* Pretty-prints the linked list */
void printRectList(rectList list) {
    rectNode* curr = list;
    int i = 0;

    while( curr != NULL ) {
        printf("%d\nsize: ", i);
        VectorPrint(curr->value.size);
        printf("pos: ");
        VectorPrint(curr->value.position);
        curr = curr->next;
        i++;
    }
}

/* Allocates the memory for the new node */
rectNode* addRectNode(rectList list) {
    rectNode* curr = list;
    while ( curr->next != NULL ) {
        curr = curr->next;
    }

    rectNode* newNode = malloc(sizeof(rectNode));
    PUSH_LT(lt, newNode, free);
    newNode->next = NULL;
    newNode->value = (editorRect) { (Vector) { 0, 0 }, (Vector) { 0, 0 }, 0};

    curr->next = newNode;
    return newNode;
}

/* Returns the amount of nodes in the linked list */
int rectListLength(rectList list) {
    printf("%p\n", list);
    rectNode* curr = list;
    int i = 0;
    while ( curr != NULL ) {
        curr = curr->next;
        i++;
    }
    return i;
}

/* Indexing function to reduce the amount of for loops in the code */
editorRect* getListRect(rectList list, int index) {
    rectNode* curr = list;
    for ( int i = 0; i < index; i++ ) {
        curr = curr->next;
    }
    return &(curr->value);
}

/* Used in UI to check when the mouse hovers over a clickable box */
int isMouseInRect(editorRect r) {
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    return (mouseX >= r.position.x && mouseX <= r.position.x + r.size.x) &&
        (mouseY >= r.position.y && mouseY <= r.position.y + r.size.y);
}

/* Same behaviour as isMouseInRect but "overloaded" for SDL_Rect */
int isMouseInSDL_Rect(SDL_Rect r) {
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    return (mouseX >= r.x && mouseX <= r.x + r.w) &&
        (mouseY >= r.y && mouseY <= r.y + r.h);
}

/* Will make it's own copy of fp (the file path). The copy is freed with freeMapFile 
 * but not the original fp */
mapFile loadMapFile(char* fp) {
    mapFile output;
    int fpLen = strlen(fp) + 1;
    output.path = malloc(fpLen * sizeof(char));
    PUSH_LT(lt, output.path, free);
    output.rl = NULL;
    strcpy(output.path, fp);

    char* buf = malloc(sizeof(char) * 1024);
    PUSH_LT(lt, buf, free);
    FILE* f;
    Vector pos;
    Vector size;
    f = fopen(fp, "r");
    PUSH_LT(lt, f, fclose);
    if ( !f ) {
        POP_LT(lt);
        POP_LT(lt);
        output.path = NULL;
        return output;
    }
    char* start;
    char* end;

    while ( fgets(buf, 1024, f) ) {
        if ( ferror(f) ) {
            freeAllRectNodes(output.rl);
            POP_LT_PTR(lt, buf);
            POP_LT_PTR(lt, f);
            POP_LT_PTR(lt, output.path);
            output.path = NULL;
            return output;
        }
        start = buf;
        pos.x = strtod(start, &end);
        start = end;
        pos.y = strtod(start, &end);
        start = end;
        size.x = strtod(start, &end);
        start = end;
        size.y = strtod(start, &end);
        start = end;

        rectNode* newNode;
        /* Adding new rect to list */
        if ( output.rl == NULL ) {
            /* First rect */
            newNode = malloc(sizeof(rectNode));
            PUSH_LT(lt, newNode, free);
            newNode->next = NULL;
            output.rl = newNode;

        } else {
            newNode = addRectNode(output.rl);
        }
        if ( size.x < 0 ) {
            size.x *= -1;
            pos.x -= size.x;
        }
        if ( size.y < 0 ) {
            size.y *= -1;
            pos.y -= size.y;
        }
        newNode->value.position = pos;
        newNode->value.size = size;
        newNode->value.selected = 0;
    }

    POP_LT_PTR(lt, buf);
    POP_LT_PTR(lt, f);

    return output;
}

/* Deallocator for mapFile. Deallocates the mapFile struct and all of it's related 
 * memory allocations such as it's linked list.
 */
void freeMapFile(mapFile* mf) {
    POP_LT_PTR(lt, mf->path);
    freeAllRectNodes(mf->rl);
}

/* Saves a given mapFile to it's associated path on the disk */
void saveMapFile(mapFile* mf) {
    FILE* f = fopen(mf->path, "w");
    PUSH_LT(lt, f, fclose);
    if ( !f ) {
        POP_LT_PTR(lt, f);
        return;
    }
    rectNode* curr = mf->rl;
    while ( curr != NULL ) {
        fprintf(f, "%f %f %f %f\n", curr->value.position.x, curr->value.position.y, curr->value.size.x, curr->value.size.y);
        curr = curr->next;
    }

    POP_LT_PTR(lt, f);
}

/* Used when typing text into the file path box, appends a letter to the string */
void mapAddPathChar(int i, mapEditorState* es) {
    int len = strlen(es->mf->path) + 2;
    char* new = malloc(len * sizeof(char));
    PUSH_LT(lt, new, free);
    strcpy(new, es->mf->path);
    new[len-1] = '\0';
    new[len-2] = i;
    POP_LT_PTR(lt, es->mf->path);
    es->mf->path = new;
}

/* Reads the state of relevant keyboard keys and performs the appropriate 
 * procedures to handle them.
 */
void mapHandleInput(int* KEYS, mapEditorState* es) {
    // TODO: Something happens when deleting after exiting and reentering the editor
    if ( es->currTool == TYPING_PATH ) {
        for ( int i = 97; i < 123; i++ ) {
            if ( KEYS[i] ) {
                mapAddPathChar(i, es);
                KEYS[i] = 0;
            }
        }
        if ( KEYS[8] ) {
            int len = strlen(es->mf->path);
            if ( len != 0 ) {
                char* new = malloc(len * sizeof(char));
                PUSH_LT(lt, new, free);
                strncpy(new, es->mf->path, len - 1);
                new[len-1] = '\0';
                POP_LT_PTR(lt, es->mf->path);
                es->mf->path = new;
                KEYS[8] = 0;
            }
        }
        if ( KEYS[46] ) {
            /* . */
            mapAddPathChar(46, es);
            KEYS[46] = 0;
        }
        if ( KEYS[32] ) {
            /* Space */
            mapAddPathChar(32, es);
            KEYS[32] = 0;
        }
        if ( KEYS[13] ) {
            /* Return */
            KEYS[13] = 0;
            es->currTool = SELECT;
        }
        if ( KEYS[45] ) {
            /* - */
            mapAddPathChar(45, es);
            KEYS[45] = 0;
        }
        return;
    }
    if ( KEYS[SDLK_j] ) {
        /* J - Grid Size- */
        if ( es->gridSize > 2 ) {
            es->gridSize--;
            es->tmpGridSize = es->gridSize;
            if ( CTRL_STATE ) {
                KEYS[SDLK_j] = 0;
            }
        }
    }
    if ( KEYS[SDLK_k] ) {
        /* K - Grid Size+ */
        if ( es->gridSize > 1 ) {
            es->gridSize++;
            es->tmpGridSize = es->gridSize;
            if ( CTRL_STATE ) {
                KEYS[SDLK_k] = 0;
            }
        }
    }
    if ( KEYS[SDLK_g] ) {
        /* G - Grid Toggle */
        if ( es->gridSize == 1 ) {
            es->gridSize = es->tmpGridSize;
        } else {
            es->gridSize = 1;
        }
        KEYS[SDLK_g] = 0;
    }
    if ( KEYS[115] ) {
        /* S - select */
        es->currTool = SELECT;
        puts("Select");
    }
    if ( KEYS[114] ) {
        /* R - rect  */
        es->currTool = RECT;
        puts("Rect");
    }
    if ( KEYS[SDLK_m] ) {
        /* M - load new map */
        es->currTool = LOAD_CONFIRM;
    }
    if ( KEYS[SDLK_n] ) {
        /* N - save map */
        es->currTool = SAVE_CONFIRM;
    }
    switch ( es->currTool ) {
    case SELECT:
        /* X - delete */
        if ( KEYS[120] ) {
            rectNode* currNode = es->rl;
            rectNode* nextNode = NULL;
            if ( currNode != NULL) {
                nextNode = es->rl->next;
                if ( currNode->value.selected ) {
                    POP_LT_PTR(lt, currNode);
                    es->rl = nextNode;
                } else {
                    int i = 0;
                    while ( nextNode != NULL ) {
                        if ( nextNode->value.selected ) {
                            popRectNode(es->rl, i + 1);
                            i -= 1;
                            printf("Current Node: %p Next Node. %p\n", currNode, currNode->next);
                        }
                        i++;
                        puts("Reassigned");
                        if ( currNode->next == NULL ) {
                        puts("null");
                            nextNode = NULL;
                        } else {
                        puts("Not null");
                        /* TODO: Something weird happens here sometimes */
                        currNode = currNode->next;
                            nextNode = currNode->next;
                        }
                    }
                    puts("Done");
                }
                es->mf->rl = es->rl;
            }
        }
        /* Z - delete all rects */
        if ( KEYS[SDLK_z] ) {
            freeAllRectNodes(es->rl);
            es->rl = NULL;
        }

    case RECT:
        break;
    case SAVE_CONFIRM:
        if ( KEYS[13] || KEYS[SDLK_e] ) {
            saveMapFile(es->mf);
            es->currTool = SAVE_DONE;
            KEYS[13] = 0;
            KEYS[SDLK_e] = 0;
        }
        if ( KEYS[27] ) {
            es->currTool = SELECT;
        }
        break;
    case SAVE_DONE:
        if ( KEYS[13] || KEYS[27] || KEYS[SDLK_e] ) {
            es->currTool = SELECT;
        }
        break;
    case LOAD_CONFIRM:
        if ( KEYS[13] || KEYS[SDLK_e] ) {
            mapFile newMap = loadMapFile(es->mf->path);
            freeMapFile(es->mf);
            es->mf->path = newMap.path;
            es->mf->rl = newMap.rl;
            es->rl = newMap.rl;
            es->currTool = LOAD_DONE;
            KEYS[13] = 0;
            KEYS[SDLK_e] = 0;
        }
        if ( KEYS[27] ) {
            es->currTool = SELECT;
        }
        break;
    case LOAD_DONE:
        if ( KEYS[13] || KEYS[27] || KEYS[SDLK_e] ) {
            es->currTool = SELECT;
        }
        break;
    }
}

/* Updates the mapEditorState according to what mouse button has been pressed */
void mapHandleMouseClick(int button, mapEditorState* es, worldRenderer* renderer) {
    if ( button == 2 ) {
        /* View panning */
        if ( es->currTool != PAN ) {
            es->prevTool = es->currTool;
            es->currTool = PAN;
            int mX, mY;
            SDL_GetMouseState(&mX, &mY);
            es->panStartPos.x = mX;
            es->panStartPos.y = mY;
            es->panStartCamera = renderer->position;
        }
    }
    switch ( es->currTool ) {
    case SELECT:
        if ( button == 1 ) {
            if ( isMouseInSDL_Rect(es->pathRect) ) {
                /* Clicking in the path window to type a new path */
                es->currTool = TYPING_PATH;
            }
            rectNode* currNode = es->rl;
            editorRect currRect;
            while ( currNode != NULL ) {
                currRect = currNode->value;
                VectorSubIp(&(currRect.position), renderer->position);
                if ( isMouseInRect(currRect) ) {
                    currNode->value.selected = 1;
                } else {
                    currNode->value.selected = 0;
                }
                currNode = currNode->next;
            }
        }
        break;
    case RECT:
        if ( button == 1 ) {
            if ( es->cursorState == 0 ) {
                es->cursorState = 1;
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);
                es->prevMousePos.x = roundf((mouseX + renderer->position.x) / es->gridSize) * es->gridSize;
                es->prevMousePos.y = roundf((mouseY + renderer->position.y) / es->gridSize) * es->gridSize;
            } else if ( es->cursorState == 1 ) {
                es->cursorState = 0;
                rectNode* newNode;
                /* Adding new rect to list */
                if ( es->rl == NULL ) {
                    /* First rect */
                    newNode = malloc(sizeof(rectNode));
                    PUSH_LT(lt, newNode, free);
                    newNode->next = NULL;
                    es->rl = newNode;

                } else {
                    newNode = addRectNode(es->rl);
                }
                Vector newPos = es->prevMousePos;
                Vector newSize;
                newSize.x = roundf((es->mousePos.x + renderer->position.x - es->prevMousePos.x) / es->gridSize) * es->gridSize;
                newSize.y = roundf((es->mousePos.y + renderer->position.y - es->prevMousePos.y) / es->gridSize) * es->gridSize;
                if ( newSize.x < 0 ) {
                    newSize.x *= -1;
                    newPos.x -= newSize.x;
                }
                if ( newSize.y < 0 ) {
                    newSize.y *= -1;
                    newPos.y -= newSize.y;
                }
                newNode->value.position = newPos;
                newNode->value.size = newSize;
                newNode->value.selected = 0;
            }
        }
        if ( button == 3 ) {
            /* Right Click */
            if ( es->cursorState == 1 ) {
                es->cursorState = 0;
            }
        }
        break;
    case TYPING_PATH:
        if ( button == 1 ) {
            es->currTool = SELECT;
        }
        break;
    }
}

/* Updates the mapEditorState according to what mouse button has been released */
void mapHandleMouseRelease(int button, mapEditorState* es, worldRenderer* renderer) {
    if ( button == 2 ) {
        /* View panning */
        es->currTool = es->prevTool;
    }
}

/* Updates the mapEditorState, should be called each frame the editor is open */
void mapEditUpdate(mapEditorState* es, worldRenderer* renderer) {
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    es->mousePos.x = mouseX;
    es->mousePos.y = mouseY;
    if ( es->currTool == PAN ) {
        int mX, mY;
        SDL_GetMouseState(&mX, &mY);
        Vector mDelta = (Vector) { mX, mY };
        renderer->position = es->panStartCamera;
        VectorAddIp(&(renderer->position), es->panStartPos);
        VectorSubIp(&(renderer->position), mDelta);
    }
    sprintf(es->gridSizeText, "Grid size: %i", es->tmpGridSize);
}

/* Draws the entire screen while the map editor is open. The world and UI are
 * drawn on separate rendering layers which are combined at the end of the 
 * function before being drawn to the internal screen buffer.
 */
void mapEditDraw(worldRenderer* renderer, mapEditorState* es) {
    /* Render Things in World */
    es->cursorRect->x = es->mousePos.x;
    es->cursorRect->y = es->mousePos.y;
    if ( es->cursorState == 1 ) {
        Vector rectSize;
        rectSize.x = roundf((es->mousePos.x + renderer->position.x - es->prevMousePos.x) / es->gridSize) * es->gridSize;
        rectSize.y = roundf((es->mousePos.y + renderer->position.y - es->prevMousePos.y) / es->gridSize) * es->gridSize;
        renderRect(renderer, 0x66FFE49E, es->prevMousePos, rectSize);
    }
    rectNode* currNode = es->rl;
    while ( currNode != NULL ) {
        if ( currNode->value.selected ) {
            renderRect(renderer, SEL_RECT_COLOR, currNode->value.position, currNode->value.size);
        } else {
            renderRect(renderer, EDIT_RECT_COLOR, currNode->value.position, currNode->value.size);
        }
        currNode = currNode->next;
    }
    /* ---------------------- */

    /* Render UI */
    Vector wrldPos = renderer->position;
    renderer->position = (Vector) { 0, 0 };
    SDL_SetRenderTarget(renderer->renderer, uiLayer.tx);
    SDL_SetRenderDrawColor(renderer->renderer, 0x0, 0x0, 0x0, 0x0);
    SDL_RenderClear(renderer->renderer);
    SDL_RenderCopy(renderer->renderer, es->cursorTexture, NULL, es->cursorRect);

    /* Render Grid */
    if ( es->gridSize != 1 ) {
        for ( int y = -((int) wrldPos.y % es->gridSize); y < SCREEN_HEIGHT; y += es->gridSize ) {
            SDL_SetRenderDrawColor(renderer->renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
            SDL_RenderDrawLine(renderer->renderer, 0, y, SCREEN_WIDTH, y);
        }
        for ( int x = -((int) wrldPos.x % es->gridSize); x < SCREEN_WIDTH; x += es->gridSize ) {
            SDL_SetRenderDrawColor(renderer->renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
            SDL_RenderDrawLine(renderer->renderer, x, 0, x, SCREEN_HEIGHT);
        }
    }
    /* ----------- */
    
    /* Render top right corner */
    renderRect(renderer, 0x82000000, (Vector) { SCREEN_WIDTH - 200, 0 }, (Vector) { 200, 150 });
    renderRect(renderer, 0x82000000, (Vector) { 0, 0 }, (Vector) { 230, 150 });
    SDL_Rect lastRect = renderText(renderer, "MAP EDITOR", TRIGHT, (Vector) { SCREEN_WIDTH * 0.99, 0 }, (SDL_Color) { 0xFF, 0xFF, 0xFF });
    SDL_Rect keyRect = (SDL_Rect) { lastRect.x - 6, lastRect.y + lastRect.h - 8, TKEYSIZE * 3, TKEYSIZE * 3 };
    renderKeyboardKey(renderer, 18, keyRect);
    if ( es->currTool == SELECT ) {
        lastRect = renderText(renderer, "Select", TLEFT, (Vector) { lastRect.x + TKEYSIZE * 3, lastRect.y + lastRect.h + 1 }, (SDL_Color) { 0xaa, 0xFF, 0xaa });
    } else {
        lastRect = renderText(renderer, "Select", TLEFT, (Vector) { lastRect.x + TKEYSIZE * 3, lastRect.y + lastRect.h + 1 }, (SDL_Color) { 0xFF, 0xFF, 0xFF });
    }
    keyRect = (SDL_Rect) { lastRect.x - TKEYSIZE * 3 - 6, lastRect.y + lastRect.h - 8, TKEYSIZE * 3, TKEYSIZE * 3 };
    renderKeyboardKey(renderer, 17, keyRect);
    if ( es->currTool == RECT ) {
        lastRect = renderText(renderer, "Rect", TLEFT, (Vector) { lastRect.x, lastRect.y + lastRect.h + 1 }, (SDL_Color) { 0xaa, 0xFF, 0xaa });
    } else {
        lastRect = renderText(renderer, "Rect", TLEFT, (Vector) { lastRect.x, lastRect.y + lastRect.h + 1 }, (SDL_Color) { 0xFF, 0xFF, 0xFF });
    }
    /* ----------------------- */

    /* Render top left corner */
    lastRect = renderTextSmall(renderer, es->mf->path, TLEFT, (Vector) { 10, 10 }, (SDL_Color) { 0xFF, 0xFF, 0xFF });
    lastRect.x -= 5; lastRect.y -= 2; lastRect.w += 20; lastRect.h += 6;
    if ( es->currTool == TYPING_PATH ) {
        renderRect(renderer, 0xFF615151, (Vector) { lastRect.x, lastRect.y }, (Vector) { lastRect.w, lastRect.h });
    } else {
        renderRect(renderer, 0xFF413131, (Vector) { lastRect.x, lastRect.y }, (Vector) { lastRect.w, lastRect.h });
    }
    es->pathRect = lastRect;
    lastRect = renderTextSmall(renderer, es->mf->path, TLEFT, (Vector) { 10, 10 }, (SDL_Color) { 0xFF, 0xFF, 0xFF });
    //lastRect.x -= 5; lastRect.y -= 2; lastRect.w += 20;
    lastRect.y -= 2;
    lastRect.h += 6;
    lastRect = renderText(renderer, "Save map", TLEFT, (Vector) { lastRect.x, lastRect.y + lastRect.h + 5 }, (SDL_Color) { 0xFF, 0xFF, 0xFF });
    keyRect = (SDL_Rect) { 160, lastRect.y - 10, TKEYSIZE * 3, TKEYSIZE * 3 };
    renderKeyboardKey(renderer, 13, keyRect);
    lastRect = renderText(renderer, "Load map", TLEFT, (Vector) { lastRect.x, lastRect.y + lastRect.h }, (SDL_Color) { 0xFF, 0xFF, 0xFF });
    keyRect = (SDL_Rect) { 160, lastRect.y - 10, TKEYSIZE * 3, TKEYSIZE * 3 };
    renderKeyboardKey(renderer, 12, keyRect);
    /* ---------------------- */

    /* Grid Controls */
    renderRect(renderer, 0x82000000, (Vector) { 0, SCREEN_HEIGHT - 180 }, (Vector) { 250, 200 });
    lastRect = renderText(renderer, es->gridSizeText, TLEFT, (Vector) { 6, SCREEN_HEIGHT - 175 }, (SDL_Color) { 0xFF, 0xFF, 0xFF });
    lastRect = renderText(renderer, "Toggle Grid", TLEFT, (Vector) { 66, SCREEN_HEIGHT - 130 }, (SDL_Color) { 0xFF, 0xFF, 0xFF });
    keyRect = (SDL_Rect) { 6, lastRect.y - 10, TKEYSIZE * 3, TKEYSIZE * 3 };
    renderKeyboardKey(renderer, 6, keyRect);
    lastRect = renderText(renderer, "Grid Size +", TLEFT, (Vector) { 66, lastRect.y + lastRect.h }, (SDL_Color) { 0xFF, 0xFF, 0xFF });
    keyRect = (SDL_Rect) { 6, lastRect.y - 10, TKEYSIZE * 3, TKEYSIZE * 3 };
    renderKeyboardKey(renderer, 9, keyRect);
    lastRect = renderText(renderer, "Grid Size -", TLEFT, (Vector) { 66, lastRect.y + lastRect.h }, (SDL_Color) { 0xFF, 0xFF, 0xFF });
    keyRect = (SDL_Rect) { 6, lastRect.y - 10, TKEYSIZE * 3, TKEYSIZE * 3 };
    renderKeyboardKey(renderer, 10, keyRect);
    /* ------------- */

    /* Popups */
    if ( es->currTool == SAVE_CONFIRM ) {
        renderConfirmPopup(renderer, "Save file?");
    }
    if ( es->currTool == SAVE_DONE ) {
        renderPopup(renderer, "File saved!");
    }
    if ( es->currTool == LOAD_CONFIRM ) {
        renderConfirmPopup(renderer, "Load file?");
    }
    if ( es->currTool == LOAD_DONE ) {
        renderPopup(renderer, "File loaded!");
    }
    /* ----- */

    SDL_SetRenderTarget(renderer->renderer, NULL);
    SDL_RenderCopy(renderer->renderer, uiLayer.tx, NULL, NULL);
    renderer->position = wrldPos;
}
