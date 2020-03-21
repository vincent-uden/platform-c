#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "../headers/lifetime.h"
#include "../headers/pause.h"
#include "../headers/player.h"
#include "../headers/physics.h"
#include "../headers/rendering.h"
#include "../headers/map_edit.h"
#include "../headers/world.h"

const int SCREEN_WIDTH = 1024;
const int SCREEN_HEIGHT = 768;
const int TKEYSIZE = 16;
const int FPSARRSIZE = 60;

int CTRL_STATE = 0;

enum game_mode {PLAYING, MAPEDIT, PAUSED, FRAMESTEP};
/* Pause menu values */
const int pauseMenuLen = 4;
const char* menuTexts[] = {"Resume", "Controls", "Settings", "Exit"};
/* ----------------- */

renderLayer uiLayer;
renderLayer pauseLayer;

Lifetime lt;
TTF_Font* sansBold;
TTF_Font* sansBoldSmall;
TTF_Font* sansBoldBig;
TTF_Font* sansBoldHuge;
SDL_Texture* keyboardTexture;

int main() {
    lt = createLt();

    /* SDL Initialization */
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
    /* ------------------ */

    /* Render layers */
    worldRenderer wrldRenderer = { renderer, (Vector) { 0, 0 } };
    worldRenderer* wRenderer = &wrldRenderer;
    uiLayer = createRenderLayer(wRenderer, (Vector) { SCREEN_WIDTH, SCREEN_HEIGHT });
    pauseLayer = createRenderLayer(wRenderer, (Vector) { SCREEN_WIDTH, SCREEN_HEIGHT });
    /* ------------- */

    /* Texture loading */
    SDL_Texture* cursorTexture = IMG_LoadTexture(renderer, "./textures/cursor.png");
    PUSH_LT(lt, cursorTexture, SDL_DestroyTexture);
    SDL_Texture* bgTexture = IMG_LoadTexture(renderer, "./textures/bg.png");
    PUSH_LT(lt, bgTexture, SDL_DestroyTexture);
    SDL_Rect cursorRect, bgRect;
    cursorRect.x = 0;
    cursorRect.y = 0;
    cursorRect.w = 12;
    cursorRect.h = 21;
    bgRect.x = 0;
    bgRect.y = 0;
    bgRect.w = 512;
    bgRect.h = 512;
    keyboardTexture = IMG_LoadTexture(renderer, "./textures/keys.png");
    PUSH_LT(lt, keyboardTexture, SDL_DestroyTexture);
    /* --------------- */

    /* Font Loading */
    TTF_Init();
    sansBold      = TTF_OpenFont("./textures/fonts/SourceSansPro-Regular.ttf", 32);
    sansBoldSmall = TTF_OpenFont("./textures/fonts/SourceSansPro-Regular.ttf", 22);
    sansBoldBig   = TTF_OpenFont("./textures/fonts/SourceSansPro-Regular.ttf", 46);
    sansBoldHuge  = TTF_OpenFont("./textures/fonts/SourceSansPro-Regular.ttf", 60);
    /* ------------ */

    Player player = makePlayer((Vector) { 100, 100 });

    int running = 1;
    int frames = FPSARRSIZE;
    SDL_Event e; 
    
    int KEYS[322];
    for ( int i = 0; i < 322; i++ ) {
        KEYS[i] = 0;
    }
    int MBUTTONS[5];
    for ( int i = 0; i < 5; i++ ) {
        MBUTTONS[i] = 0;
    }
    int MBUTTONSR[5];
    for ( int i = 0; i < 5; i++ ) {
        MBUTTONSR[i] = 0;
    }

    Uint64 NOW = SDL_GetPerformanceCounter();
    Uint64 LAST = 0;
    double deltaTime = 0;
    double timeHistory[FPSARRSIZE];
    for ( int i = 0; i < FPSARRSIZE; i++ ) {
        timeHistory[i] = 0.01;
    }
    char fpsText[10];

    enum game_mode gm = PLAYING;

    mapEditorState editorState;
    editorState.prevMousePos = (Vector) { 0, 0 };
    editorState.mousePos = (Vector) { 0, 0 };
    editorState.rl = NULL;
    editorState.cursorTexture = cursorTexture; editorState.cursorRect = &cursorRect;
    editorState.cursorState = 0;
    editorState.gridSize = 64;
    editorState.tmpGridSize = editorState.gridSize;
    editorState.currTool = SELECT;
    editorState.mf = NULL;
    editorState.gridSizeText = malloc(sizeof(char) * 20);
    PUSH_LT(lt, editorState.gridSizeText, free);

    worldState gameState;
    gameState.rects = NULL;
    gameState.rectAmount = 0;
    pauseMenuState pauseState;
    pauseState.selectedIndex = 0;

    mapFile testMap = loadMapFile("./test-map.txt");
    editorState.rl = testMap.rl;
    editorState.mf = &testMap;
    worldSetRects(&gameState, &editorState);

    running = 1;
    int pauseReturn;
    int frameStep = 0;

    while ( running ) {
        while ( SDL_PollEvent(&e) != 0 ) {
            switch ( e.type ) {
            case SDL_QUIT:
                running = 0;
                break;
            case SDL_KEYDOWN:
                if ( e.key.keysym.sym ==  SDLK_f ) {
                    if ( gm == PLAYING ) {
                        gm = MAPEDIT;
                    } else if ( gm == MAPEDIT && editorState.currTool != TYPING_PATH ) {
                        gm = PLAYING;
                        worldSetRects(&gameState, &editorState);
                    }
                }
                if ( e.key.keysym.sym == SDLK_ESCAPE ) {
                    if ( gm == PLAYING ) {
                        gm = PAUSED;
                    } else if ( gm == PAUSED ) {
                        gm = PLAYING;
                    } else if ( gm == MAPEDIT ) {
                        if ( editorState.currTool != SAVE_DONE &&
                             editorState.currTool != SAVE_CONFIRM &&
                             editorState.currTool != LOAD_DONE &&
                             editorState.currTool != LOAD_CONFIRM
                        ) {
                            gm = PLAYING;
                        }
                    }
                }
                if ( e.key.keysym.sym < 322  && e.key.keysym.sym >= 0 )
                    KEYS[e.key.keysym.sym] = 1;
                if ( e.key.keysym.sym == SDLK_p ) {
                    if ( gm == PLAYING ) {
                        gm = FRAMESTEP;
                        frameStep = 0;
                    } else if ( gm == FRAMESTEP ) {
                        gm = PLAYING;
                    }
                }
                if ( e.key.keysym.sym == SDLK_n ) {
                    frameStep = 1;
                }
                if ( e.key.keysym.sym == SDLK_LCTRL ) {
                    CTRL_STATE = 1;
                }
                break;
            case SDL_KEYUP:
                if ( e.key.keysym.sym < 322  && e.key.keysym.sym >= 0 ) {
                    KEYS[e.key.keysym.sym] = 0;
                }
                if ( e.key.keysym.sym == SDLK_LCTRL ) {
                    CTRL_STATE = 0;
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                MBUTTONS[e.button.button - 1] = 1;
                break;
            case SDL_MOUSEBUTTONUP:
                MBUTTONSR[e.button.button - 1] = 1;
                break;
            default:
                break;
            }
        }

        LAST = NOW;
        NOW = SDL_GetPerformanceCounter();
        deltaTime = (double) ((NOW - LAST) / (double)SDL_GetPerformanceFrequency() );
        /* The zero check is only to prevent zero-division errors from timeHistory */
        if ( deltaTime > 0.1 || deltaTime == 0) {
            deltaTime = 0.1;
        }
        if ( frames % 10 == 0) {
            if ( frames >= FPSARRSIZE ) {
                frames = 0;
            }
            double total = 0;
            for ( int i = 0; i < FPSARRSIZE; i++ ) {
                total += timeHistory[i];
            }
            total /= FPSARRSIZE;
            sprintf(fpsText, "%i", (int) (1.0 / total));
        }
        timeHistory[frames] = deltaTime;

        /* Updating */
        switch (gm) {
        case PLAYING:
            worldUpdate(&gameState);
            playerHandleInput(&player, KEYS);
            for ( int i = 0; i < 10; i++ ) {
                playerUpdate(&player, deltaTime / 10, &gameState);
            }
            wrldRenderer.position = VectorSub(player.position, (Vector) { SCREEN_WIDTH / 2 - PLAYERSIZE / 2, SCREEN_HEIGHT / 2 - PLAYERSIZE / 2});
            break;
        case FRAMESTEP:
            if ( frameStep ) {
                worldUpdate(&gameState);
                playerHandleInput(&player, KEYS);
                for ( int i = 0; i < 10; i++ ) {
                    playerUpdate(&player, deltaTime / 10, &gameState);
                }
                wrldRenderer.position = VectorSub(player.position, (Vector) { SCREEN_WIDTH / 2 - PLAYERSIZE / 2, SCREEN_HEIGHT / 2 - PLAYERSIZE / 2});
                frameStep = 0;
            }
            break;
        case MAPEDIT:
            for ( int i = 0; i < 5; i++ ) {
                if ( MBUTTONS[i] ) {
                    mapHandleMouseClick(i + 1, &editorState, wRenderer);
                    MBUTTONS[i] = 0;
                }
                if ( MBUTTONSR[i] ) {
                    mapHandleMouseRelease(i + 1, &editorState, wRenderer);
                    MBUTTONSR[i] = 0;
                }
            }
            mapHandleInput(KEYS, &editorState);
            mapEditUpdate(&editorState, wRenderer);
            break;
        case PAUSED:
            pauseReturn = pauseMenuHandleInput(KEYS, &pauseState);
            if ( pauseReturn == -1 ) {
                running = 0;
            }
            if ( pauseReturn == 1 ) {
                gm = PLAYING;
            }
            break;
        default:
            break;
        }

        /* Clear Screen */
        SDL_SetRenderDrawColor(renderer, 0x31, 0x31, 0x31, 0xFF);
        SDL_RenderClear(renderer);
        renderBackground(wRenderer, bgTexture, bgRect);

        switch (gm) {
        case PLAYING:
            renderRect(wRenderer, RECT_SHADOW, VectorAdd(player.position, (Vector) { 5, 10 }), (Vector) { PLAYERSIZE, PLAYERSIZE });
            renderRect(wRenderer, 0xFF404dFF, player.position, (Vector) { PLAYERSIZE, PLAYERSIZE });
            worldDraw(wRenderer, &gameState);
            break;
        case FRAMESTEP:
            renderRect(wRenderer, RECT_SHADOW, VectorAdd(player.position, (Vector) { 5, 10 }), (Vector) { PLAYERSIZE, PLAYERSIZE });
            renderRect(wRenderer, 0xFF404dFF, player.position, (Vector) { PLAYERSIZE, PLAYERSIZE });
            renderTextSmall(wRenderer, "Frame-by-frame mode (P-Close mode, N-Next frame)", TRIGHT, VectorAdd(wRenderer->position, (Vector) { SCREEN_WIDTH * 0.99, 0 }), (SDL_Color) { 0xFF, 0x33, 0x33 });
            worldDraw(wRenderer, &gameState);
            break;
        case MAPEDIT:
            renderRect(wRenderer, 0xFF404dFF, player.position, (Vector) { PLAYERSIZE, PLAYERSIZE });
            mapEditDraw(wRenderer, &editorState);
            break;
        case PAUSED:
            renderRect(wRenderer, RECT_SHADOW, VectorAdd(player.position, (Vector) { 5, 10 }), (Vector) { PLAYERSIZE, PLAYERSIZE });
            renderRect(wRenderer, 0xFF404dFF, player.position, (Vector) { PLAYERSIZE, PLAYERSIZE });
            worldDraw(wRenderer, &gameState);
            pauseMenuDraw(&pauseState, wRenderer);
            break;
        }

        renderTextSmall(wRenderer, fpsText, TLEFT, VectorAdd(wRenderer->position, (Vector) { 0, SCREEN_HEIGHT - 25 }), (SDL_Color) { 0x00, 0xFF, 0x00 });

        SDL_RenderPresent(renderer);
        frames++;
    }

    exit(0);
}
