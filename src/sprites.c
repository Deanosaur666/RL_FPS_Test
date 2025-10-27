#include "sprites.h"

// sprites
int spriteend = 0;
int spritemax = 4;
Texture2D* spritetex;
Rectangle* spriterect;
Vector2* spritesize;
Vector2* spriteorigin;


void reallocSprites(int bbcount) {
	spritetex = (Texture2D *)realloc(spritetex, sizeof(Texture2D) * bbcount);
	spriterect = (Rectangle *)realloc(spriterect, sizeof(Rectangle) * bbcount);
	spritesize = (Vector2 *)realloc(spriterect, sizeof(Vector2) * bbcount);
	spriteorigin = (Vector2 *)realloc(spriterect, sizeof(Vector2) * bbcount);
}

void unloadSprites() {
	for(int i = 0; i < spritemax; i ++) {
		Texture2D tex = spritetex[i];
		UnloadTexture(tex);
	}

	free(spritetex);
	free(spriterect);
	free(spritesize);
	free(spriteorigin);
}

// bool fliph, Vector 2 origin
int addSprite(const char* texfile, float scale) {
	int spriteindex = spriteend;
	spriteend ++;

	if(spriteindex > spritemax) {
		spritemax *= 2;
		reallocSprites(spritemax);
	}

	Texture2D tex = LoadTexture(texfile);
	Rectangle rect = { 0.0f, 0.0f, (float)tex.width,  (float)tex.height};
	Vector2 size = { rect.width, rect.height };
	size = Vector2Scale(size, scale);
	Vector2 origin = { size.x/2, 0.0f };

	spritetex[spriteindex] = tex;
	spriterect[spriteindex] = rect;
	spritesize[spriteindex] = size;
	spriteorigin[spriteindex] = origin;

	return spriteindex;
}