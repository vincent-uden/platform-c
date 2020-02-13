#include "../headers/pause.h"

void pauseMenuHandleInput(int* KEYS, pauseMenuState* ps) {
    /* Up */
    if ( KEYS[SDLK_w] ) {
        if ( ps->selectedIndex > 0 ) {
            ps->selectedIndex--;
        }
        KEYS[SDLK_w] = 0;
    }
    /* Down */
    if ( KEYS[SDLK_s] ) {
        if ( ps->selectedIndex < pauseMenuLen - 1 ) {
            ps->selectedIndex++;
        }
        KEYS[SDLK_s] = 0;
    }
}

void pauseMenuUpdate(pauseMenuState* ps, worldRenderer* renderer) {

}

void pauseMenuDraw(pauseMenuState* ps, worldRenderer* renderer) {
    Vector wrldPos = renderer->position;
    renderer->position = (Vector) { 0, 0 };
    SDL_SetRenderTarget(renderer->renderer, pauseLayer.tx);
    SDL_SetRenderDrawColor(renderer->renderer, 0x0, 0x0, 0x0, 0x0);
    SDL_RenderClear(renderer->renderer);

    renderRect(renderer, 0x44000000, (Vector) { 0, 0 }, (Vector) { SCREEN_WIDTH, SCREEN_HEIGHT });

    SDL_Rect lastRect = (SDL_Rect) { 10, 40, 0, 0 };
    for ( int i = 0; i < pauseMenuLen; i++ ) {
        if ( i == ps->selectedIndex ) {
            lastRect = renderTextHuge(renderer, (char*) menuTexts[i], TLEFT, (Vector) { 50, lastRect.y + lastRect.h}, (SDL_Color) { 0xFF, 0xFF, 0xFF });
        } else {
            lastRect = renderText(renderer, (char*) menuTexts[i], TLEFT, (Vector) { 40, lastRect.y + lastRect.h}, (SDL_Color) { 0xFF, 0xFF, 0xFF, 0xcc});
        }
    }

    lastRect = renderTextSmall(renderer, "Platform C - Alpha version 0.1", TRIGHT, (Vector) { SCREEN_WIDTH - 10, 10}, (SDL_Color) { 0xDD, 0xFF, 0xFF});
    lastRect = renderTextSmall(renderer, "github.com/vincent-uden/platform-c", TRIGHT, (Vector) { SCREEN_WIDTH - 10, SCREEN_HEIGHT - 40}, (SDL_Color) { 0xDD, 0xFF, 0xFF});

    SDL_SetRenderTarget(renderer->renderer, NULL);
    SDL_RenderCopy(renderer->renderer, pauseLayer.tx, NULL, NULL);
    renderer->position = wrldPos;
}
