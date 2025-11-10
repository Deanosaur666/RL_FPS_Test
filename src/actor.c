
#include "actor.h"

void drawActor(Camera camera, Actor actor) {
    Sprite sprite;
    if(actor.isSpinner) {
        sprite = spinners[actor.sname]
            [getSpinnerIndex(camera, actor.position, actor.angle)];
    }
    else {
        sprite = sprites[actor.sname];
    }
    drawBBSprite(camera, sprite, actor.position);
}