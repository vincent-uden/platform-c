#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "../headers/lifetime.h"
#include "../headers/player.h"
#include "../headers/physics.h"
#include "../headers/rendering.h"
#include "../headers/map_edit.h"
#include "../headers/world.h"

const int SCREEN_WIDTH = 1024;
const int SCREEN_HEIGHT = 768;

enum game_mode {PLAYING, MAPEDIT};

Lifetime lt;
TTF_Font* sansBold;
TTF_Font* sansBoldSmall;

int main() {
    lt = createLt();

    SDL_Window* window = NULL;

    SDL_Renderer* renderer = NULL;

    if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
        printf("Failed to create SDL context");
        exit(1);
    }

    window = SDL_CreateWindow("SDL Tutorial", 
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            SCREEN_WIDTH,
            SCREEN_HEIGHT,
            SDL_WINDOW_SHOWN
    );

    if ( window == NULL ) {
        printf("Failed to get SDL window");
        exit(1);
    }
    PUSH_LT(lt, window, SDL_DestroyWindow);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE | SDL_RENDERER_PRESENTVSYNC);
    if ( renderer == NULL ) {
        printf("Failed to create SDL Renderer");
        UNWIND_LT(lt);
        exit(1);
    }
    PUSH_LT(lt, renderer, SDL_DestroyRenderer);
    SDL_ShowCursor(0);

    /* Texture loading */
    SDL_Texture* cursorTexture = IMG_LoadTexture(renderer, "./textures/cursor.png");
    PUSH_LT(lt, cursorTexture, SDL_DestroyTexture);
    SDL_Rect cursorRect;
    cursorRect.x = 0;
    cursorRect.y = 0;
    cursorRect.w = 12;
    cursorRect.h = 21;
    /* End of texture loading */

    /* Font Loading */
    TTF_Init();
    sansBold = TTF_OpenFont("./textures/fonts/SourceSansPro-Regular.ttf", 32);
    sansBoldSmall = TTF_OpenFont("./textures/fonts/SourceSansPro-Regular.ttf", 22);
    /* End of font loading */

    Player player = makePlayer((Vector) { 100, 100 });

    int running = 1;
    int frames = 0;
    SDL_Event e; 
    
    int KEYS[322];
    for ( int i = 0; i < 322; i++ ) {
        KEYS[i] = 0;
    }
    int MBUTTONS[5];
    for ( int i = 0; i < 5; i++ ) {
        MBUTTONS[i] = 0;
    }

    Uint64 NOW = SDL_GetPerformanceCounter();
    Uint64 LAST = 0;
    double deltaTime = 0;

    enum game_mode gm = PLAYING;

    mapEditorState editorState;
    editorState.prevMousePos = (Vector) { 0, 0 };
    editorState.mousePos = (Vector) { 0, 0 };
    editorState.rl = NULL;
    editorState.cursorTexture = cursorTexture; editorState.cursorRect = &cursorRect;
    editorState.cursorState = 0;
    editorState.currTool = SELECT;

    worldState gameState;
    gameState.rects = NULL;
    gameState.rectAmount = 0;

    mapFile testMap = loadMapFile("./test-map.txt");
    editorState.rl = testMap.rl;
    worldSetRects(&gameState, &editorState);

    while ( running ) {
        while ( SDL_PollEvent(&e) != 0 ) {
            switch ( e.type ) {
            case SDL_QUIT:
                running = 0;
                break;
            case SDL_KEYDOWN:
                // Paragraph to switch to map editor
                if ( e.key.keysym.sym ==  SDLK_f ) {
                    if ( gm == PLAYING ) {
                        gm = MAPEDIT;
                    } else if ( gm == MAPEDIT ) {
                        gm = PLAYING;
                        worldSetRects(&gameState, &editorState);
                    }
                }
                if ( e.key.keysym.sym < 322  && e.key.keysym.sym >= 0 )
                    KEYS[e.key.keysym.sym] = 1;
                break;
            case SDL_KEYUP:
                if ( e.key.keysym.sym < 322  && e.key.keysym.sym >= 0 )
                    KEYS[e.key.keysym.sym] = 0;
                break;
            case SDL_MOUSEBUTTONDOWN:
                MBUTTONS[e.button.button - 1] = 1;
                break;
            case SDL_MOUSEBUTTONUP:
                break;
            default:
                break;
            }
        }

        LAST = NOW;
        NOW = SDL_GetPerformanceCounter();
        deltaTime = (double) ((NOW - LAST) / (double)SDL_GetPerformanceFrequency() );

        /* Updating */
        switch (gm) {
        case PLAYING:
            worldUpdate(&gameState);
            playerHandleInput(&player, KEYS);
            for ( int i = 0; i < 10; i++ ) {
                playerUpdate(&player, deltaTime / 10, &gameState);
            }
            break;
        case MAPEDIT:
            for ( int i = 0; i < 5; i++ ) {
                if ( MBUTTONS[i] ) {
                    mapHandleMouseClick(i + 1, &editorState);
                    MBUTTONS[i] = 0;
                }
            }
            mapHandleInput(KEYS, &editorState);
            mapEditUpdate(&editorState);
            break;
        default:
            break;
        }

        /* Clear Screen */
        SDL_SetRenderDrawColor(renderer, 0x31, 0x31, 0x31, 0xFF);
        SDL_RenderClear(renderer);

        switch (gm) {
        case PLAYING:
            worldDraw(renderer, &gameState);
            renderRect(renderer, 0xFF404dFF, player.position, (Vector) { PLAYERSIZE, PLAYERSIZE });
            break;
        case MAPEDIT:
            renderRect(renderer, 0xFF404dFF, player.position, (Vector) { PLAYERSIZE, PLAYERSIZE });
            mapEditDraw(renderer, &editorState);
            break;
        }

        SDL_RenderPresent(renderer);
        frames++;

        if ( frames % 100 == 0 )
            printLt(lt);
    }

    exit(0);
}
