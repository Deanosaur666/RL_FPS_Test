#include "sprites.h"
#include "vectors.h"

Sprite sprites[spr_MAX];
Sprite spinners[spn_MAX][8];


void unloadSprites() {
	for(int i = 0; i < spr_MAX; i ++) {
		Texture2D tex = sprites[i].tex;
		UnloadTexture(tex);
	}
}

void addSprite(const char* texfile, SpriteName sname, float scale) {
	sprites[sname] = addSprite_Pro(texfile, sname, scale, false);
}

Sprite addSprite_Pro(const char* texfile, SpriteName sname, float scale, bool flipH) {

	Texture2D tex = LoadTexture(texfile);
	Rectangle rect = { 0.0f, 0.0f, (float)tex.width,  (float)tex.height};
	Vector2 size = { rect.width, rect.height };
	size = Vector2Scale(size, scale);
	if(flipH)
		size.x *= -1;
	Vector2 origin = { size.x/2, 0.0f };

	 return (Sprite) { tex, rect, size, origin };
}

void addSpinner(const char* tex1, const char* tex2, const char* tex3, const char* tex4, const char* tex5,
		SpinnerName sname, float scale) {
	spinners[sname][0] = addSprite_Pro(tex1, sname, scale, false); // S
	spinners[sname][1] = addSprite_Pro(tex2, sname, scale, false); // SE
	spinners[sname][2] = addSprite_Pro(tex3, sname, scale, false); // E
	spinners[sname][3] = addSprite_Pro(tex4, sname, scale, false); // NE
	spinners[sname][4] = addSprite_Pro(tex5, sname, scale, false); // N
	spinners[sname][5] = addSprite_Pro(tex4, sname, scale, true); // NW
	spinners[sname][6] = addSprite_Pro(tex3, sname, scale, true); // W
	spinners[sname][7] = addSprite_Pro(tex2, sname, scale, true); // SW
}

void initSprites() {
    addSprite("Mod.png", spr_Mod, 2.0f / 700.0f);
	addSprite("Peach.png", spr_Peach, 2.0f / 700.0f);
	addSprite("Dog.png", spr_Dog, 2.0f / 1400.0f);

	addSpinner(	"Rat0000.png",
				"Rat0001.png",
				"Rat0002.png",
				"Rat0003.png",
				"Rat0004.png",
				spn_Rat, 2.0f / 700.0f
				);
}

unsigned int getSpinnerIndex(Camera camera, Vector3 spn_pos, float spn_angle) {
	float angleTo = atan2f(spn_pos.z - camera.position.z, spn_pos.x - camera.position.x);
	//float camAngle = atan2f(camera.target.z - camera.position.z, camera.target.x - camera.position.x);
	float angle = fmod((angleTo - spn_angle) + PI, PI*2);
	return (int) roundf((angle / (PI/4))) % 8;

}

void drawBBSprite(Camera camera, Sprite sprite, Vector3 pos) {
	DrawBillboardPro(camera, sprite.tex, sprite.rect, pos, up, sprite.size,
		sprite.origin, 0.0f, WHITE);
}