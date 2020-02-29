#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL2_rotozoom.h>
#include <stdio.h>
#include <math.h>
#include <stdint.h>

#include "./physics.h"
#include "./lifetime.h"

extern TTF_Font* sansBold;
extern TTF_Font* sansBoldSmall;
extern TTF_Font* sansBoldBig;
extern TTF_Font* sansBoldHuge;
extern SDL_Texture* keyboardTexture;
extern Lifetime lt;
extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;
extern const int TKEYSIZE;

enum textAdjust {TLEFT, TRIGHT};

typedef struct {
  SDL_Renderer* renderer;

  Vector position;
} worldRenderer;

typedef struct {
    SDL_Texture* tx;

    Vector position;
    Vector size;
} renderLayer;

renderLayer createRenderLayer(worldRenderer* renderer, Vector size);

void renderRect(worldRenderer* renderer, int color, Vector pos, Vector size);
void renderBackground(worldRenderer* renderer, SDL_Texture* bgTexture, SDL_Rect txtRect);
void renderKeyboardKey(worldRenderer* renderer, int key, SDL_Rect dst);

SDL_Rect renderTextBackend(worldRenderer* renderer, char* text, enum textAdjust adj, Vector pos, SDL_Color color, TTF_Font* font);
SDL_Rect renderText(worldRenderer* renderer, char* text, enum textAdjust adj, Vector pos, SDL_Color color);
SDL_Rect renderTextSmall(worldRenderer* renderer, char* text, enum textAdjust adj, Vector pos, SDL_Color color);
SDL_Rect renderTextBig(worldRenderer* renderer, char* text, enum textAdjust adj, Vector pos, SDL_Color color);
SDL_Rect renderTextHuge(worldRenderer* renderer, char* text, enum textAdjust adj, Vector pos, SDL_Color color);

SDL_Rect renderPopup(worldRenderer* renderer, char* text);
SDL_Rect renderConfirmPopup(worldRenderer* renderer, char* text);
