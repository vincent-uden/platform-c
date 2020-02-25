#pragma once

#include <SDL2/SDL.h>

#include "./physics.h"
#include "./world.h"

#define PLAYERSIZE 30

typedef struct {
    Vector position;
    Vector speed;
    Vector acc;

    double mass;
    double grav;
    double air_k;

    int jumpsLeft;
    int holdingJump;
} Player;

Player makePlayer(Vector pos);

void playerHandleInput(Player* player, int* KEYS);
void playerUpdate(Player* player, double deltaTime, worldState* ws);
