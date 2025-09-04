#include "camera/engine_camera.h"

#include <stdio.h>
#include <stdlib.h> 
#include <stdbool.h>
#include <string.h>
#include <math.h>

#define PI 3.14159265358979323846


Camera CurrentCamera;
bool Inited = false;

Camera* InitCamera(float Position[3], float Orientation[2])
{
    memcpy(CurrentCamera.position, Position, sizeof(CurrentCamera.position));
    memcpy(CurrentCamera.orientation, Orientation, sizeof(CurrentCamera.orientation));

    Inited = true;

    return &CurrentCamera;
}


void TranslateCameraIncrement(float movement_added[3])
{
    if (Inited)
    {
        float new_array[3] = {
            CurrentCamera.position[0]-movement_added[0],
            CurrentCamera.position[1]-movement_added[1],
            CurrentCamera.position[2]+movement_added[2]
        };
        memcpy(CurrentCamera.position, new_array, sizeof(CurrentCamera.position));
    } else {
        printf("Warning! Camera System has not been Initialized");
    }

}

void TranslateCameraByAngleIncrement(float velocity, char *dir)
{
    if (Inited)
    {
        float pitch = CurrentCamera.orientation[0];
        float yaw = CurrentCamera.orientation[1];
        
        float NormalVector[3] = {};
        if (strcmp(dir, "FORWARD") == 0) 
        {
            float ForwardVector[3] = {
                cosf(pitch) * sinf(yaw),  
                sinf(pitch),               
                cosf(pitch) * cosf(yaw)  
            };
            memcpy(NormalVector, ForwardVector, sizeof(ForwardVector));
        } else if (strcmp(dir, "BACKWARD") == 0) {
            float BackwardVector[3] = {
                cosf(pitch ) * sinf(yaw ) * -1,  
                sinf(pitch ) * -1,               
                cosf(pitch ) * cosf(yaw ) * -1  
            };
            memcpy(NormalVector, BackwardVector, sizeof(BackwardVector));
        } else if (strcmp(dir, "LEFT") == 0) {
            float RightVector[3] = {
                sinf(yaw - (PI / 2)),
                0,
                cosf(yaw - (PI / 2))
            };
            memcpy(NormalVector, RightVector, sizeof(RightVector));
        } else if (strcmp(dir, "RIGHT") == 0) {
            float LeftVector[3] = {
                sinf(yaw + (PI / 2)),
                0,
                cosf(yaw + (PI / 2))
            };
            memcpy(NormalVector, LeftVector, sizeof(LeftVector));
        } else {
            printf("Warning! Invalid direction for TranslateCameraByAngleIncrement\n");
            return;
        }
     

        float new_array[3] = {
            CurrentCamera.position[0] + (velocity * NormalVector[0]),
            CurrentCamera.position[1] - (velocity * NormalVector[1]),
            CurrentCamera.position[2] + (velocity * NormalVector[2]) 
        };

        memcpy(CurrentCamera.position, new_array, sizeof(CurrentCamera.position));
    } else {
        printf("Warning! Camera System has not been Initialized");
    }
}

void RotateCameraIncrement(float orientation_added[2])
{
    if (Inited)
    {
        float new_array[2] = {
            CurrentCamera.orientation[0]+orientation_added[0],
            CurrentCamera.orientation[1]+orientation_added[1]
        };
        memcpy(CurrentCamera.orientation, new_array, sizeof(CurrentCamera.orientation));
    } else {
        printf("Warning! Camera System has not been Initialized");
    }
}

Camera* ObtainRecord()
{
    if (Inited)
    {
        return &CurrentCamera;
    } else {
        printf("Warning! Camera System has not been Initialized (ObtainRecord Failure)");
    }
}