#include "headers.h"
#include "sprites.h"
#include "vectors.h"
#include "actor.h"

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

// actors
Actor* actors;
int actorEnd = 0;
int actorMax = 4;

void reallocActors(int count) {
	actors = (Actor *)realloc(actors, sizeof(Actor) * count);
}

void freeActors() {
	free(actors);
}

void addActor(int sname, Vector3 pos, bool spinner) {
	int actorIndex = actorEnd;
	actorEnd ++;

	if(actorEnd > actorMax) {
		actorMax *= 2;
		reallocActors(actorMax);
	}
	// placing on ground
	Vector3 raypos = Vector3Add(pos, Vector3Scale(up, 16.0f));

	Ray ray = { raypos, down };
	RayCollision collision = RayToMap(ray, mapmodel);

	if(collision.hit) {
		pos = collision.point;
	}

	// adding sprite and pos to lists
	Actor actor = { sname, pos, 0, spinner };

	actors[actorIndex] = actor;

}

typedef struct ActorDist {
	int actorIndex;
	float distance;
} ActorDist;

int compareActorDist(const void* a, const void* b) {
	ActorDist actorA = *(ActorDist *)a;
	ActorDist actorB = *(ActorDist *)b;

	return (int)(actorB.distance*1000.0f - actorA.distance*1000.0f);
}

typedef Vector3 Position;

int main () {

	ecs_world_t * world = ecs_init();

	ECS_COMPONENT(world, Vector3);
	ECS_COMPONENT(world, Position);

	ecs_entity_t e = ecs_new(world);
	ecs_set(world, e, Vector3, {0, 0, 0});
	//ecs_set(world, e, Position, {0, 0, 0});

	printf("%b\n", ecs_has(world, e, Vector3));
	printf("%b\n", ecs_has(world, e, Position));


	// Tell the window to use vsync and work on high DPI displays
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

	// Create the window and OpenGL context
	InitWindow(1280, 800, "FPS Test");

	// Utility function from resource_dir.h to find the resources folder and set it as the current working directory so we can load from it
	SearchAndSetResourceDir("resources");

	initSprites();

	// Define the camera to look into our 3d world
    Camera camera = { 0 };
    camera.position = (Vector3){ 0.0f, 1.8f, 0.0f };    // Camera position
    camera.target = (Vector3){ 0.0f, 1.8f, -1.0f };      // Camera looking at point
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 45.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type

	// Load gltf model
    //mapmodel = LoadModel("map1.gltf");
	mapmodel = LoadModel("map2.gltf");
	//mapmodel = LoadModel("Bunny.glb");
	Vector3 mappos = { 0.0f, 0.0f, 0.0f };

	// actors
	actors = (Actor *)malloc(sizeof(Actor) * actorMax);
	
	addActor(spr_Mod, (Vector3){ 3.0f, 0.0f, 0.0f }, false);
	addActor(spr_Peach, (Vector3){ -3.0f, 0.0f, 0.0f }, false);
	addActor(spr_Peach, (Vector3){ 0.0f, 0.0f, 3.0f }, false);
	addActor(spr_Dog, (Vector3){ 0.0f, 0.0f, 0.0f }, false);

	addActor(spn_Rat, (Vector3){ -3.0f, 0.0f, -5.0f }, true);
	addActor(spn_Rat, (Vector3){ 3.0f, 0.0f, -5.0f }, true);

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

				ActorDist actorDists[actorEnd];
				for(int a = 0; a < actorEnd; a ++) {
					actorDists[a] = (ActorDist){ a, Vector3Distance(camera.position, actors[a].position) };
				}
				qsort(actorDists, actorEnd, sizeof(ActorDist), compareActorDist);

				// actors
				for(int i = 0; i < actorEnd; i ++) {
					int actorIndex = actorDists[i].actorIndex;
					Actor actor = actors[actorIndex];
					drawActor(camera, actor);
				}

			EndMode3D();
		
			DrawFPS(10, 10);
		
		EndDrawing();
        //----------------------------------------------------------------------------------
	}

	// cleanup
	UnloadModel(mapmodel);
	unloadSprites();
	freeActors();

	ecs_fini(world);

	// destroy the window and cleanup the OpenGL context
	CloseWindow();
	return 0;
}
