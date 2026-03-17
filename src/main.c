#include <stdio.h>

#include "rEngine.h"


float CameraMovementSpeed = 0.01f;
int quit = 1;

float vertices[8][3] = {
    {0,0,0},
    {1,0,0},
    {0,1,0},
    {1,1,0},
    {0,0,1},
    {1,0,1},
    {0,1,1},
    {1,1,1}
};

int faces[6][4] = {
    {0, 1, 3, 2}, // Front  (z = 0)
    {4, 5, 7, 6}, // Back   (z = 1)
    {0, 1, 5, 4}, // Bottom (y = 0)
    {2, 3, 7, 6}, // Top    (y = 1)
    {0, 2, 6, 4}, // Left   (x = 0)
    {1, 3, 7, 5}  // Right  (x = 1)
};

int colors[6][3] = {
    {255, 0, 0},   // Front  - Red
    {0, 255, 0},   // Back   - Green
    {0, 0, 255},   // Bottom - Blue
    {255, 255, 0}, // Top    - Yellow
    {255, 0, 255}, // Left   - Magenta
    {0, 255, 255}  // Right  - Cyan
};

void MoveCamera(float mouse_x, float mouse_y)
{

    const Uint8 *state = SDL_GetKeyboardState(NULL);

    if (state[SDL_SCANCODE_O])
    {
        quit = 0;
    }

    if (state[SDL_SCANCODE_S])
    {
        char * dir = "BACKWARD";
        TranslateCameraByAngleIncrement(CameraMovementSpeed, dir);
    } else if (state[SDL_SCANCODE_W]) {
        char * dir = "FORWARD";
        TranslateCameraByAngleIncrement(CameraMovementSpeed, dir);
    }

    if (state[SDL_SCANCODE_D])
    {
        char * dir = "RIGHT";
        TranslateCameraByAngleIncrement(CameraMovementSpeed, dir);
    } else if (state[SDL_SCANCODE_A]) {
        char * dir = "LEFT";
        TranslateCameraByAngleIncrement(CameraMovementSpeed, dir);
    }

    if (state[SDL_SCANCODE_E])
    {
        float move_up[3] = {0,CameraMovementSpeed,0};
        TranslateCameraIncrement(move_up);
    } else if (state[SDL_SCANCODE_Q]) {
        float move_down[3] = {0,-CameraMovementSpeed,0};
        TranslateCameraIncrement(move_down);
    }
    
    float Rotate[2] = {
        -mouse_y*0.002f, mouse_x*0.002f // Eventually make it Engine fix the flipping issue
    };
    RotateCameraIncrement(Rotate);
}

int main(int argc, char *argv[])
{
    Engine *engine = Init("rEngine Example", 1280, 720); // Initialize the engine with a window title and dimensions

    Object3D * myObject = CreateObject3D(vertices, 8, faces, 6, colors, 6); // Create a 3D object

    float position[3] = {0,0,-3};
    float orientation[2] = {0,0};
    Camera* MyCamera = InitCamera(position, orientation);

    SDL_Event event;
    SDL_SetRelativeMouseMode(SDL_TRUE);
    while (1) {
        EngineRenderColor(engine, 50,50,50, 255);

        EngineRenderClear(engine);

        DisplayObject3D(engine, myObject);

        float dx = 0;
        float dy = 0;

        while (SDL_PollEvent(&event) != 0) {
            switch (event.type) {
                case SDL_MOUSEMOTION:
                    dx += event.motion.xrel; 
                    dy += event.motion.yrel;
                    break; 
                case SDL_QUIT:
                    goto QUIT;
            }
        }
        MoveCamera(dx, dy);
        
        if (quit == 0) {
            goto QUIT;
        }
        // event and quits

        EngineUpdate(engine);
    }
    QUIT:
    Cleanup(engine);
    return 0;
}