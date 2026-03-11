#ifndef RENGINE
#define RENGINE
#define SDL_MAIN_HANDLED


#include "SDL.h"

#include "core/object.h"
#include "core/matrix.h"
#include "core/camera.h"

// Base Structs
typedef struct Engine {
    SDL_Window* window;
    SDL_Renderer* renderer;
} Engine;


// Core
Engine * Init(const char * window_name, const int window_width, const int window_height);

void DisplayObject3D(Engine * engine, Object3D * Object);

void EngineUpdate(Engine * engine);
void EngineRenderColor(Engine * engine, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
void EngineRenderClear(Engine * engine);
int EventHandler(Engine * engine);

void Cleanup(Engine * engine);

// Additionals
void SetRelativeMouseMode(int enabled);
#endif 