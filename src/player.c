#include "../headers/player.h"

Player makePlayer(Vector pos) {
    return (Player) {
        pos,
        (Vector) { 0, 0 },
        (Vector) { 0, 0 },
        1000,
        9.82,
        20,
        0
    };
}

void playerHandleInput(Player* player, int* KEYS) {
    player->acc = (Vector) { 0, 0 };
    if ( KEYS[SDLK_d] ) {
        player->acc.x += 50000;
    }
    if ( KEYS[SDLK_a] ) {
        player->acc.x -= 50000;
    }
    if ( KEYS[SDLK_SPACE] ) {
        if ( player->jumpsLeft > 0 ) {
            player->acc.y -= player->grav * 20000;
            player->jumpsLeft--;
        }
    }
}

void playerUpdate(Player* player, double deltaTime, worldState* ws) {
    double airResMag = player->air_k * (player->speed.x * player->speed.x);
    double airResDir = 1;
    if ( player->speed.x < 0 ) {
        airResDir = -1;
    }

    Vector airRes = (Vector) { -airResMag * airResDir, 0 };

    VectorAddIp(&(player->acc), VectorDiv(airRes, player->mass));

    VectorAddIp(&(player->speed), VectorMul(player->acc, deltaTime));
    VectorAddIp(&player->speed, VectorMul((Vector) { 0, player->grav * 1500 }, deltaTime));
    /* Some clamping to avoid minimal sliding */
    if ( fabs(player->speed.x) < 40 ) {
        player->speed.x = 0;
    }
    if ( fabs(player->speed.y) < 40 ) {
        // player->speed.y = 0;
    }

    VectorAddIp(&(player->position), VectorMul(player->speed, deltaTime));

    /* (AABB) Collision with environment */
    Vector topLeft = player->position;
    Vector topRight = VectorAdd(player->position, (Vector) { PLAYERSIZE, 0 });
    Vector botLeft = VectorAdd(player->position, (Vector) { 0, PLAYERSIZE });
    Vector botRight = VectorAdd(player->position, (Vector) { PLAYERSIZE, PLAYERSIZE });

    worldRect currRect;
    for ( int i = 0; i < ws->rectAmount; i++ ) {
        currRect = ws->rects[i];
        if ( topLeft.x    > currRect.position.x
             && topLeft.x < currRect.position.x + currRect.size.x
             && topLeft.y > currRect.position.y
             && topLeft.y < currRect.position.y + currRect.size.y
        ) {
            float pushX = currRect.position.x + currRect.size.x - topLeft.x;
            float pushY = currRect.position.y + currRect.size.y - topLeft.y;

            if ( fabs(pushX) < fabs(pushY) ) {
                player->position.x = currRect.position.x + currRect.size.x;
                player->speed.x = 0;
            } else {
                player->position.y = currRect.position.y + currRect.size.y;
                if ( player->speed.y > 0 ) {
                    player->speed.y = 0;
                    player->jumpsLeft = 1;
                } else {
                    player->speed.y = 0;
                    player->acc.y = 0;
                }
            }
        }

        if ( topRight.x    > currRect.position.x
             && topRight.x < currRect.position.x + currRect.size.x
             && topRight.y > currRect.position.y
             && topRight.y < currRect.position.y + currRect.size.y
        ) {
            float pushX =  currRect.position.x - topRight.x;
            float pushY = currRect.position.y + currRect.size.y - topLeft.y;

            if ( fabs(pushX) < fabs(pushY) ) {
                player->position.x = currRect.position.x - PLAYERSIZE;
                player->speed.x = 0;
            } else {
                player->position.y = currRect.position.y + currRect.size.y;
                if ( player->speed.y > 0 ) {
                    player->speed.y = 0;
                    player->jumpsLeft = 1;
                }else {
                    player->speed.y = 0;
                    player->acc.y = 0;
                }
            }
        }

        if ( botLeft.x    > currRect.position.x
             && botLeft.x < currRect.position.x + currRect.size.x
             && botLeft.y > currRect.position.y
             && botLeft.y < currRect.position.y + currRect.size.y
        ) {
            float pushX = currRect.position.x + currRect.size.x - botLeft.x;
            float pushY = currRect.position.y - botLeft.y;

            if ( fabs(pushX) < fabs(pushY) ) {
                player->position.x = currRect.position.x + currRect.size.x;
                player->speed.x = 0;
            } else {
                player->position.y = currRect.position.y - PLAYERSIZE;
                if ( player->speed.y > 0 ) {
                    player->speed.y = 0;
                    player->jumpsLeft = 1;
                }else {
                    player->speed.y = 0;
                    player->acc.y = 0;
                }
            }
        }

        if ( botRight.x    > currRect.position.x
             && topRight.x < currRect.position.x + currRect.size.x
             && botRight.y > currRect.position.y
             && botRight.y < currRect.position.y + currRect.size.y
        ) {
            float pushX =  currRect.position.x - botRight.x;
            float pushY = currRect.position.y - botRight.y;

            if ( fabs(pushX) < fabs(pushY) ) {
                player->position.x = currRect.position.x - PLAYERSIZE;
                player->speed.x = 0;
            } else {
                player->position.y = currRect.position.y - PLAYERSIZE;
                if ( player->speed.y > 0 ) {
                    player->speed.y = 0;
                    player->jumpsLeft = 1;
                }else {
                    player->speed.y = 0;
                    player->acc.y = 0;
                }
            }
        }
    }
}
