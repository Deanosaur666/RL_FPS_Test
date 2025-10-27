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

#endif