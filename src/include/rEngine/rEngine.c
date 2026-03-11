#include "rengine.h"

#include <stdio.h>

Engine EngineInstance;

SDL_Event event;

Engine * Init(const char * window_name, const int window_width, const int window_height)
{
    SDL_Init(SDL_INIT_VIDEO);

    EngineInstance.window = SDL_CreateWindow(window_name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_width, window_height, SDL_WINDOW_SHOWN);
    EngineInstance.renderer = SDL_CreateRenderer(EngineInstance.window, -1, SDL_RENDERER_ACCELERATED);

    if (EngineInstance.window == NULL){
        fprintf(stderr, "Failed to create window: %s\n", SDL_GetError());
        return NULL;
    } 

    if (EngineInstance.renderer == NULL) {
        fprintf(stderr, "Failed to create renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(EngineInstance.window);
        return NULL;
    }

    float AspectRatio = (float)window_height / (float)window_width;
    int WS[2] = {window_width, window_height};

    SetupPlane(
        (float)1000.0f,
        (float)0.1f,
        AspectRatio,
        WS
    );

    return &EngineInstance;
}


void EngineUpdate(Engine * engine)
{
    SDL_RenderPresent(engine->renderer);
}

int EventHandler(Engine * engine)
{
    while (SDL_PollEvent(&event) != 0) {
        if (event.type == SDL_QUIT) {
            return 0;
        }
    }
    return 1;
}

void EngineRenderColor(Engine * engine, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
    SDL_SetRenderDrawColor(engine->renderer, r, g, b, a);
}

void EngineRenderClear(Engine * engine)
{
    SDL_RenderClear(engine->renderer);
}


void Cleanup(Engine * engine)
{
    SDL_DestroyRenderer(engine->renderer);
    SDL_DestroyWindow(engine->window);
    SDL_Quit();

    FreeObjects3D();
}