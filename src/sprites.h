#ifndef _sprites
#define _sprites

#include "headers.h"

extern int spriteend;
extern int spritemax;
extern Texture2D* spritetex;
extern Rectangle* spriterect;
extern Vector2* spritesize;
extern Vector2* spriteorigin;

int addSprite(const char* texfile, float scale);
void unloadSprites();

void initSprites();

extern int spr_Mod;
extern int spr_Peach;
extern int spr_Dog;

#endif