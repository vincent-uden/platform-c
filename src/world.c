#include "../headers/world.h"

/* Deallocates all of the old rects ( if there are any ), then allocates and
 * sets new ones in the world state. */
void worldSetRects(worldState* ws, mapEditorState* es) {
   free(ws->rects);
   ws->rectAmount = 0;

   rectNode* currNode = es->rl;
   int amount = 0;
   while ( currNode != NULL ) {
       currNode = currNode->next;
       amount++;
   }

   worldRect* newRects = malloc(amount * sizeof(worldRect));

   currNode = es->rl;
   int i = 0;
   while ( currNode != NULL ) {
       newRects[i].position = currNode->value.position;
       newRects[i].size = currNode->value.size;
       currNode = currNode->next;
       i++;
   }
   ws->rects = newRects;
   ws->rectAmount = amount;
}

void worldUpdate(worldState* ws) {
    return;
}

void worldDraw(SDL_Renderer* renderer, worldState* ws) {
    for ( int i = 0; i < ws->rectAmount; i++ ) {
        renderRect(renderer, RECT_COLOR, ws->rects[i].position, ws->rects[i].size);
    }
}
