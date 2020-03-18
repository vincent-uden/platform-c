#include "../headers/player.h"

Player makePlayer(Vector pos) {
    return (Player) {
        pos,
        (Vector) { 0, 0 },
        (Vector) { 0, 0 },
        1000,
        12000,
        100,
        0,
        0
    };
}

void playerHandleInput(Player* player, int* KEYS) {
    player->acc = (Vector) { 0, 0 };
    if ( KEYS[SDLK_d] ) {
        player->acc.x += 100000;
    }
    if ( KEYS[SDLK_a] ) {
        player->acc.x -= 100000;
    }
    if ( KEYS[SDLK_SPACE] ) {
        player->holdingJump = 1;
        if ( player->jumpsLeft > 0 ) {
            player->acc.y -= player->grav * 10;
            player->jumpsLeft--;
        }
    } else {
        player->holdingJump = 0;
    }
}

void playerUpdate(Player* player, double deltaTime, worldState* ws) {
    double airResMag;
    if ( player->acc.x ) {
        airResMag = player->air_k * (player->speed.x * player->speed.x);
    } else {
        airResMag = 3 * player->air_k * (player->speed.x * player->speed.x);
    }
    double airResDir = 1;
    Vector oldAcc = player->acc;
    if ( player->speed.x < 0 ) {
        airResDir = -1;
    }

    Vector airRes = (Vector) { -airResMag * airResDir, 0 };

    VectorAddIp(&(player->acc), VectorDiv(airRes, player->mass));

    VectorAddIp(&(player->speed), VectorMul(player->acc, deltaTime));
    if ( player->holdingJump ) {
        VectorAddIp(&player->speed, VectorMul((Vector) { 0, player->grav * 0.5}, deltaTime));
    } else {
        VectorAddIp(&player->speed, VectorMul((Vector) { 0, player->grav}, deltaTime));
    }
    /* Some clamping to avoid minimal sliding */
    if ( fabs(player->speed.x) < 40 ) {
        player->speed.x = 0;
    }
    if ( fabs(player->speed.y) < 40 ) {
        // player->speed.y = 0;
    }
    player->acc = oldAcc;

    VectorAddIp(&(player->position), VectorMul(player->speed, deltaTime));

    /* (AABB) Collision with environment */
    Vector topLeft = player->position;
    Vector topRight = VectorAdd(player->position, (Vector) { PLAYERSIZE, 0 });
    Vector botLeft = VectorAdd(player->position, (Vector) { 0, PLAYERSIZE });
    Vector botRight = VectorAdd(player->position, (Vector) { PLAYERSIZE, PLAYERSIZE });

    worldRect currRect;
    ColliderRect playerRect;
    playerRect.pos = player->position;
    playerRect.size = (Vector) { PLAYERSIZE, PLAYERSIZE };
    for ( int i = 0; i < ws->rectAmount; i++ ) {
        currRect = ws->rects[i];
        // The collision doesn't seem to be working
        Vector push = isColliding(worldToColliderRect(currRect), playerRect);
        if ( push.x != 0 || push.y != 0) {
            if ( fabs(push.x) < fabs(push.y) ) {
                if ( player->speed.x > 0 ) {
                    player->position.x -= push.x;
                } else {
                    player->position.x += push.x;
                }
                player->speed.x = 0;
            } else {
                if ( player->speed.y > 0 ) {
                    player->position.y -= push.y;
                    player->speed.y = 0;
                    player->jumpsLeft = 1;
                } else {
                    player->position.y += push.y;
                    player->speed.y = 0;
                    player->acc.y = 0;
                }
            }
        }
    }

     /* If the player has any y-speed here it can't have collided with a floor
      * therefore it can't jump. Jumpsleft must be set to zero.
      */
    if ( player->speed.y != 0 ) {
        player->jumpsLeft = 0;
    }
}
