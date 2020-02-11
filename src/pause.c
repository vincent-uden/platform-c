#include "../headers/pause.h"

void pauseMenuHandleInput(int* KEYS, pauseMenuState* ps) {

}

void pauseMenuUpdate(pauseMenuState* ps, worldRenderer* renderer) {

}

void pauseMenuDraw(pauseMenuState* ps, worldRenderer* renderer) {
    Vector wrldPos = renderer->position;
    renderer->position = (Vector) { 0, 0 };
    SDL_SetRenderTarget(renderer->renderer, pauseLayer.tx);
    SDL_SetRenderDrawColor(renderer->renderer, 0x0, 0x0, 0x0, 0x0);
    SDL_RenderClear(renderer->renderer);

    SDL_Rect lastRect = (SDL_Rect) { 10, 10, 0, 0 };
    for ( int i = 0; i < pauseMenuLen; i++ ) {
        lastRect = renderText(renderer, (char*) menuTexts[i], TLEFT, (Vector) { 10, lastRect.y + lastRect.h }, (SDL_Color) { 0xFF, 0xFF, 0xFF });
    }

    SDL_SetRenderTarget(renderer->renderer, NULL);
    SDL_RenderCopy(renderer->renderer, pauseLayer.tx, NULL, NULL);
    renderer->position = wrldPos;
}
