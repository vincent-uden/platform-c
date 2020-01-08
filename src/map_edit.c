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

int rectListLength(rectList list) {
    rectNode* curr = list;
    int i = 0;
    while ( curr != NULL ) {
        curr = curr->next;
        i++;
    }
    return i;
}

editorRect* getListRect(rectList list, int index) {
    rectNode* curr = list;
    for ( int i = 0; i < index; i++ ) {
        curr = curr->next;
    }
    return &(curr->value);
}

int isMouseInRect(editorRect r) {
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    return (mouseX >= r.position.x && mouseX <= r.position.x + r.size.x) &&
        (mouseY >= r.position.y && mouseY <= r.position.y + r.size.y);
}

/* Will make it's own copy of fp. The copy is freed with freeMapFile but not the original fp */
mapFile loadMapFile(char* fp) {
    mapFile output;
    int fpLen = strlen(fp);
    output.path = malloc(fpLen * sizeof(char));
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
        POP_LT_PTR(lt, buf);
        POP_LT_PTR(lt, f);
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

void freeMapFile(mapFile* mf) {
    POP_LT_PTR(lt, mf->path);
    freeAllRectNodes(mf->rl);
}

void mapHandleInput(int* KEYS, mapEditorState* es) {
    // TODO: Something happens when deleting after exiting and reentering the editor
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
            }
        }
        /* Z - delete all rects */
        if ( KEYS[SDLK_z] ) {
            freeAllRectNodes(es->rl);
            es->rl = NULL;
        }
    case RECT:
        break;
    }
}

void mapHandleMouseClick(int button, mapEditorState* es) {
    switch ( es->currTool ) {
    case SELECT:
        if ( button == 1 ) {
            rectNode* currNode = es->rl;
            while ( currNode != NULL ) {
                if ( isMouseInRect(currNode->value) ) {
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
                es->prevMousePos.x = mouseX;
                es->prevMousePos.y = mouseY;
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
                Vector newSize = VectorSub(es->mousePos, es->prevMousePos);
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
    }
}

void mapEditUpdate(mapEditorState* es) {
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    es->mousePos.x = mouseX;
    es->mousePos.y = mouseY;
}

void mapEditDraw(SDL_Renderer* renderer, mapEditorState* es) {
    es->cursorRect->x = es->mousePos.x;
    es->cursorRect->y = es->mousePos.y;
    if ( es->cursorState == 1 ) {
        renderRect(renderer, 0x66FFE49E, es->prevMousePos, VectorSub(es->mousePos, es->prevMousePos));
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

    SDL_RenderCopy(renderer, es->cursorTexture, NULL, es->cursorRect);

    SDL_Rect lastRect = renderText(renderer, "MAP EDITOR", TRIGHT, (Vector) { SCREEN_WIDTH * 0.99, 0 }, (SDL_Color) { 0xFF, 0xFF, 0xFF });
    if ( es->currTool == SELECT ) {
        lastRect = renderText(renderer, "Select (S)", TLEFT, (Vector) { lastRect.x, lastRect.y + lastRect.h + 1 }, (SDL_Color) { 0xaa, 0xFF, 0xaa });
    } else {
        lastRect = renderText(renderer, "Select (S)", TLEFT, (Vector) { lastRect.x, lastRect.y + lastRect.h + 1 }, (SDL_Color) { 0xFF, 0xFF, 0xFF });
    }
    if ( es->currTool == RECT ) {
        lastRect = renderText(renderer, "Rect (R)", TLEFT, (Vector) { lastRect.x, lastRect.y + lastRect.h + 1 }, (SDL_Color) { 0xaa, 0xFF, 0xaa });
    } else {
        lastRect = renderText(renderer, "Rect (R)", TLEFT, (Vector) { lastRect.x, lastRect.y + lastRect.h + 1 }, (SDL_Color) { 0xFF, 0xFF, 0xFF });
    }
}
