/*
Raylib example file.
This is an example main file for a simple raylib project.
Use this as a starting point or replace it with your code.

by Jeffery Myers is marked with CC0 1.0. To view a copy of this license, visit https://creativecommons.org/publicdomain/zero/1.0/

*/

#include "raylib.h"
#include "raymath.h"

#include "resource_dir.h"	// utility header for SearchAndSetResourceDir

#include <stdio.h>
#include <stdlib.h>


// billboards
int bbend = 0;
int bbmax = 4;
Texture2D* bbtex;
Rectangle* bbrect;
Vector2* bbsize;
Vector2* bborigin;

Vector3 up = { 0.0f, 1.0f, 0.0f };

void reallocBBs(int bbcount) {
	bbtex = (Texture2D *)realloc(bbtex, sizeof(Texture2D) * bbcount);
	bbrect = (Rectangle *)realloc(bbrect, sizeof(Rectangle) * bbcount);
	bbsize = (Vector2 *)realloc(bbrect, sizeof(Vector2) * bbcount);
	bborigin = (Vector2 *)realloc(bbrect, sizeof(Vector2) * bbcount);
}

void unloadBBs() {
	for(int i = 0; i < bbmax; i ++) {
		Texture2D tex = bbtex[i];
		UnloadTexture(tex);
	}

	free(bbtex);
	free(bbrect);
	free(bbsize);
	free(bborigin);
}

int addBB(const char* texfile) {
	int bbindex = bbend;
	bbend ++;

	if(bbindex > bbmax) {
		bbmax *= 2;
		reallocBBs(bbmax);
	}

	Texture2D tex = LoadTexture(texfile);
	Rectangle rect = { 0.0f, 0.0f, (float)tex.width,  (float)tex.height};
	Vector2 size = { rect.width/rect.height, 1.0f };
	Vector2 origin = { 0.0f, 0.0f };

	bbtex[bbindex] = tex;
	bbrect[bbindex] = rect;
	bbsize[bbindex] = size;
	bborigin[bbindex] = origin;

	return bbindex;
}

// entities
int entityend = 0;
int entitymax = 4;
int* entitybb;
Vector3* entitypos;

void reallocEntities(int count) {
	entitybb = (int *)realloc(entitybb, sizeof(int) * count);
	entitypos = (Vector3 *)realloc(entitypos, sizeof(Vector3) * count);
}

void freeEntities() {
	free(entitybb);
	free(entitypos);
}

int addEntity(int bb, Vector3 pos) {
	int eindex = entityend;
	entityend ++;

	if(entityend > entitymax) {
		entitymax *= 2;
		reallocEntities(entitymax);
	}

	entitybb[eindex] = bb;
	entitypos[eindex] = pos;

	return eindex;
}

int main () {
	bbtex = (Texture2D *)malloc(sizeof(Texture2D) * bbmax);
	bbrect = (Rectangle *)malloc(sizeof(Rectangle) * bbmax);
	bbsize = (Vector2 *)malloc(sizeof(Vector2) * bbmax);
	bborigin = (Vector2 *)malloc(sizeof(Vector2) * bbmax);

	entitybb = (int *)malloc(sizeof(int) * entitymax);
	entitypos = (Vector3 *)malloc(sizeof(Vector3) * entitymax);

	// Tell the window to use vsync and work on high DPI displays
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

	// Create the window and OpenGL context
	InitWindow(1280, 800, "Hello Raylib");

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
    Model map1 = LoadModel("map1.gltf");
	Vector3 mappos = { 0.0f, 0.0f, 0.0f };

	float mapscale = 1.0f / 16.0f;
	Vector3 mapscale3 = { mapscale, mapscale, mapscale };
	float maprot = 0.0f;

	// billboards
	int mod = addBB("Mod.png");
	int peach = addBB("Peach.png");
	int dog = addBB("Dog.png");

	// entities
	addEntity(mod, (Vector3){ 0.0f, 0.0f, 0.0f });
	addEntity(peach, (Vector3){ 0.0f, 0.0f, 2.0f });
	addEntity(dog, (Vector3){ 0.0f, 0.0f, -2.0f });

	SetTargetFPS(60);                   // Set our game to run at 60 frames-per-second
	DisableCursor();
	
	// game loop
	while (!WindowShouldClose())		// run the loop untill the user presses ESCAPE or presses the Close button on the window
	{
		/*
		if (IsKeyDown(KEY_LEFT))
			maprot -= 5;
		if(IsKeyDown(KEY_RIGHT))
			maprot += 5;
		*/

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
				DrawModelEx(map1, mappos, (Vector3){ 0.0f, 1.0f, 0.0f }, maprot, mapscale3, WHITE);

				// entities
				for(int i = 0; i < entityend; i ++) {
					int bb = entitybb[i];
					Vector3 pos = entitypos[i];
					Texture2D tex = bbtex[bb];
					
					//DrawBillboard(camera, tex, pos, 1, WHITE);
					DrawBillboardPro(camera, tex, bbrect[bb], pos, up, bbsize[bb], bborigin[bb],
						0.0f, WHITE);
				}

			EndMode3D();
		char msg[32];
		int msglen = sprintf(msg, "Map rotation: %.0f", maprot);
		DrawText(msg, 10, 10, 20, WHITE);
		
		EndDrawing();
        //----------------------------------------------------------------------------------
	}

	// cleanup
	UnloadModel(map1);
	unloadBBs();
	freeEntities();

	// destroy the window and cleanup the OpenGL context
	CloseWindow();
	return 0;
}
