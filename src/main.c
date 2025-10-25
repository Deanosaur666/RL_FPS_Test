#include "raylib.h"
#include "raymath.h"

#include "resource_dir.h"	// utility header for SearchAndSetResourceDir

#include <stdio.h>
#include <stdlib.h>

#undef FLT_MAX
#define FLT_MAX     340282346638528859811704183484516925440.0f     // Maximum value of a float, from bit pattern 01111111011111111111111111111111

// map
Model mapmodel;

RayCollision RayToMap(Ray ray, Model map) {
	RayCollision collision = { 0 };
	collision.distance = FLT_MAX;
	collision.hit = false;

	// Check ray collision against bounding box first, before trying the full ray-mesh test
	RayCollision boxHitInfo = GetRayCollisionBox(ray, GetMeshBoundingBox(map.meshes[0]));
	if ((boxHitInfo.hit) && (boxHitInfo.distance < collision.distance))
	{
		// Check ray collision against model meshes
		RayCollision meshHitInfo = { 0 };
		for (int m = 0; m < map.meshCount; m++)
		{
			meshHitInfo = GetRayCollisionMesh(ray, map.meshes[m], map.transform);
			if (meshHitInfo.hit)
			{
				// Save the closest hit mesh
				if ((!collision.hit) || (collision.distance > meshHitInfo.distance))
					collision = meshHitInfo;

				break;  // Stop once one mesh collision is detected, the colliding mesh is m
			}
		}
	}

	return collision;
}

// sprites
int spriteend = 0;
int spritemax = 4;
Texture2D* spritetex;
Rectangle* spriterect;
Vector2* spritesize;
Vector2* spriteorigin;

Vector3 up = { 0.0f, 1.0f, 0.0f };
Vector3 down = { 0.0f, -1.0f, 0.0f };

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

// bbs
int bbend = 0;
int bbmax = 4;
int* bbsprite;
Vector3* bbpos;

void reallocBBs(int count) {
	bbsprite = (int *)realloc(bbsprite, sizeof(int) * count);
	bbpos = (Vector3 *)realloc(bbpos, sizeof(Vector3) * count);
}

void freeBBs() {
	free(bbsprite);
	free(bbpos);
}

int addBB(int sprite, Vector3 pos) {
	int bb = bbend;
	bbend ++;

	if(bbend > bbmax) {
		bbmax *= 2;
		reallocBBs(bbmax);
	}

	Vector3 raypos = Vector3Add(pos, Vector3Scale(up, 16.0f));

	Ray ray = { raypos, down };
	RayCollision collision = RayToMap(ray, mapmodel);

	if(collision.hit) {
		pos = collision.point;
	}

	bbsprite[bb] = sprite;
	bbpos[bb] = pos;

	return bb;
}

typedef struct BBDist {
	int bb;
	float distance;
} BBDist;

int compareBB(const void* a, const void* b) {
	BBDist bba = *(BBDist *)a;
	BBDist bbb = *(BBDist *)b;

	return (int)(bbb.distance*1000.0f - bba.distance*1000.0f);
}

int main () {
	spritetex = (Texture2D *)malloc(sizeof(Texture2D) * spritemax);
	spriterect = (Rectangle *)malloc(sizeof(Rectangle) * spritemax);
	spritesize = (Vector2 *)malloc(sizeof(Vector2) * spritemax);
	spriteorigin = (Vector2 *)malloc(sizeof(Vector2) * spritemax);

	bbsprite = (int *)malloc(sizeof(int) * bbmax);
	bbpos = (Vector3 *)malloc(sizeof(Vector3) * bbmax);

	// Tell the window to use vsync and work on high DPI displays
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

	// Create the window and OpenGL context
	InitWindow(1280, 800, "FPS Test");

	// Utility function from resource_dir.h to find the resources folder and set it as the current working directory so we can load from it
	SearchAndSetResourceDir("resources");

	// Define the camera to look into our 3d world
    Camera camera = { 0 };
    camera.position = (Vector3){ 0.0f, 1.0f, 0.0f };    // Camera position
    camera.target = (Vector3){ 0.0f, 1.0f, -1.0f };      // Camera looking at point
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 45.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type

	// Load gltf model
    mapmodel = LoadModel("map1.gltf");
	Vector3 mappos = { 0.0f, 0.0f, 0.0f };

	float mapscale = 1.0f / 16.0f;
	mapmodel.transform = MatrixScale(mapscale, mapscale, mapscale);

	// billboards
	int mod = addSprite("Mod.png", 0.003f);
	int peach = addSprite("Peach.png", 0.003f);
	int dog = addSprite("Dog.png", 0.002f);

	// entities
	addBB(mod, (Vector3){ 0.0f, 0.0f, 0.0f });
	addBB(peach, (Vector3){ 1.0f, 0.0f, 0.0f });
	addBB(peach, (Vector3){ 0.0f, 0.0f, 2.0f });
	addBB(dog, (Vector3){ 0.0f, 0.0f, -2.0f });

	SetTargetFPS(60);                   // Set our game to run at 60 frames-per-second
	DisableCursor();
	
	// game loop
	while (!WindowShouldClose())		// run the loop untill the user presses ESCAPE or presses the Close button on the window
	{
		if (IsKeyDown(KEY_R)) {
			camera.position.y += 0.1f;
			camera.target.y += 0.1f;
		}
		if(IsKeyDown(KEY_F)) {
			camera.position.y -= 0.1f;
			camera.target.y -= 0.1f;
		}

		UpdateCamera(&camera, CAMERA_FIRST_PERSON);
		
		// Draw
        //----------------------------------------------------------------------------------
		BeginDrawing();

			// Setup the back buffer for drawing (clear color and depth buffers)
			ClearBackground(BLACK);

			BeginMode3D(camera);
				// map
				DrawModel(mapmodel, mappos, 1.0f, WHITE);

				BBDist bbs[bbend];
				for(int bb = 0; bb < bbend; bb ++) {
					bbs[bb] = (BBDist){ bb, Vector3Distance(camera.position, bbpos[bb]) };
				}
				qsort(bbs, bbend, sizeof(BBDist), compareBB);

				// bbs
				for(int i = 0; i < bbend; i ++) {
					int bb = bbs[i].bb;
					int sprite = bbsprite[bb];
					Vector3 pos = bbpos[bb];
					Texture2D tex = spritetex[sprite];
					
					//DrawBillboard(camera, tex, pos, 1, WHITE);
					DrawBillboardPro(camera, tex, spriterect[sprite], pos, up, spritesize[sprite], spriteorigin[sprite],
						0.0f, WHITE);
				}

			EndMode3D();
		
			DrawFPS(10, 10);
		
		EndDrawing();
        //----------------------------------------------------------------------------------
	}

	// cleanup
	UnloadModel(mapmodel);
	unloadSprites();
	freeBBs();

	// destroy the window and cleanup the OpenGL context
	CloseWindow();
	return 0;
}
