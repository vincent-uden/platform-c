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
extern Lifetime lt;

enum textAdjust {TLEFT, TRIGHT};

void renderRect(SDL_Renderer* renderer, int color, Vector pos, Vector size);

SDL_Rect renderTextBackend(SDL_Renderer* renderer, char* text, enum textAdjust adj, Vector pos, SDL_Color color, TTF_Font* font);
SDL_Rect renderText(SDL_Renderer* renderer, char* text, enum textAdjust adj, Vector pos, SDL_Color color);
SDL_Rect renderTextSmall(SDL_Renderer* renderer, char* text, enum textAdjust adj, Vector pos, SDL_Color color);

SDL_Rect renderPopup(SDL_Renderer* renderer, char* text);
SDL_Rect renderConfirmPopup(SDL_Renderer* renderer, char* text);
