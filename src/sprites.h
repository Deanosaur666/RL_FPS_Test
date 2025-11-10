#ifndef _sprites
#define _sprites

#include "headers.h"

typedef enum {
    spr_Mod,
    spr_Peach,
    spr_Dog,

    spr_MAX
} SpriteName;

typedef enum {
    spn_Rat,
    spn_MAX,
} SpinnerName;

typedef struct Sprite {
    Texture2D tex;
    Rectangle rect;
    Vector2 size;
    Vector2 origin;
} Sprite;

void initSprites();
void addSprite(const char* texfile, SpriteName sname, float scale);
Sprite addSprite_Pro(const char* texfile, SpriteName, float scale, bool flipH);

void addSpinner(const char* tex1, const char* tex2, const char* tex3, const char* tex4, const char* tex5, SpinnerName sname, float scale);

void unloadSprites();

unsigned int getSpinnerIndex(Camera camera, Vector3 spn_pos, float spn_angle);
void drawBBSprite(Camera, Sprite, Vector3);

extern Sprite sprites[spr_MAX];
extern Sprite spinners[spn_MAX][8];

#endif