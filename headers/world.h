#pragma once

#include <SDL2/SDL.h>

#include "physics.h"
#include "rendering.h"
#include "map_edit.h"

typedef struct {
    Vector position;
    Vector size;
} worldRect;

typedef struct {
    worldRect* rects;
    int rectAmount;
} worldState;

void worldSetRects(worldState* ws, mapEditorState* es);
void worldUpdate(worldState* ws);
void worldDraw(worldRenderer* renderer, worldState* ws);

ColliderRect worldToColliderRect(worldRect wr);
