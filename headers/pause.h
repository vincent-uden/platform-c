#pragma once

#include <SDL2/SDL.h>
#include <string.h>

#include "./physics.h"
#include "./rendering.h"
#include "./lifetime.h"

extern Lifetime lt;
extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;
extern renderLayer pauseLayer;
extern const int pauseMenuLen ;
extern const char* menuTexts[];

typedef struct {
    int selectedIndex;
} pauseMenuState;

void pauseMenuHandleInput(int* KEYS, pauseMenuState* ps);
void pauseMenuUpdate(pauseMenuState* ps, worldRenderer* renderer);
void pauseMenuDraw(pauseMenuState* ps, worldRenderer* renderer);
