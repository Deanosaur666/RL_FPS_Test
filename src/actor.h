#ifndef _actor
#define _actor

#include "headers.h"
#include "sprites.h"

typedef struct Actor {
    unsigned int sname;
    Vector3 position;
    float angle;
    bool isSpinner;
} Actor;

void drawActor(Camera, Actor);

#endif