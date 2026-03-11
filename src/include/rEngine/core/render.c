#include "rEngine.h"

#include <stdio.h>
#include <stdbool.h>

int FOV = 90;
bool EdgeLineEnabled = false;


typedef struct {
    int vertices[4];
    float depth;
    int faceIndex;
} FaceDepth;

int DrawPoint(float x, float y, int s, Engine * eng)
{

    int CommonErrorCode = 0;
    for (int y_ = 0; y_ < s; y_++)
    {
        for (int x_ = 0; x_ < s; x_++)
        {
            CommonErrorCode += SDL_RenderDrawPoint(eng->renderer, x+x_, y+y_);
        }
    }

    if (CommonErrorCode != 0)
    {
        CommonErrorCode = 1;
    } else {
        CommonErrorCode = 0;
    }
    return CommonErrorCode;
}

float AverageFaceZ(MatrixPoint RotatedPoints[], int face[4]) {
    float average = 0.0f;
    for (int i = 0; i < 4; i++) {
        average += RotatedPoints[face[i]].z;
    }
    return average / 4.0f;
}

int CompareFaces(const void *a, const void *b) {
    FaceDepth *fa = (FaceDepth*)a;
    FaceDepth *fb = (FaceDepth*)b;
    if (fa->depth < fb->depth) return 1;  
    if (fa->depth > fb->depth) return -1;
    return 0;
}

void RasterizeFace(int a, int b, int c, int d, Point2D ScreenPoints[], MatrixPoint CameraPoints[], float color[3], Engine * eng) {
    SDL_SetRenderDrawColor(eng->renderer, color[0], color[1], color[2], 255);

    int indices[6] = {0,1,2, 0,2,3};

    SDL_Vertex verts_one[3] = {
         {{ScreenPoints[a].x, ScreenPoints[a].y}, {color[0], color[1],color[2],255}, {0,0}},
         {{ScreenPoints[b].x, ScreenPoints[b].y}, {color[0], color[1],color[2],255}, {1,0}},
         {{ScreenPoints[c].x, ScreenPoints[c].y}, {color[0], color[1],color[2],255}, {1,1}}
    };

    SDL_Vertex verts_two[3] = {
         {{ScreenPoints[a].x, ScreenPoints[a].y}, {color[0], color[1],color[2],255}, {0,0}},
         {{ScreenPoints[c].x, ScreenPoints[c].y}, {color[0], color[1],color[2],255}, {1,1}},
         {{ScreenPoints[d].x, ScreenPoints[d].y}, {color[0], color[1],color[2],255}, {0,1}}
    };
    
    SDL_RenderGeometry(eng->renderer, NULL, verts_one, 3, NULL, 0);
    SDL_RenderGeometry(eng->renderer, NULL, verts_two, 3, NULL, 0);
    //SDL_RenderGeometry(renderer, NULL, verts, 4, indices, 6);

}


void ObjectSimpleFaceHandle(
    Object3D* Object,
    MatrixPoint CameraPoints[],
    Point2D ScreenPoints[],
    Engine * eng
) {
    int numFaces = Object->face_count;
    FaceDepth faces[numFaces];

    for (int i = 0; i < numFaces; i++) {
        for (int j = 0; j < 4; j++) {
            faces[i].vertices[j] = Object->faces[i][j];
        }
        faces[i].depth = AverageFaceZ(CameraPoints, Object->faces[i]);
        faces[i].faceIndex = i;
    }

    qsort(faces, numFaces, sizeof(FaceDepth), CompareFaces);

    for (int i = 0; i < numFaces; i++) {
        int a = faces[i].vertices[0];
        int b = faces[i].vertices[1];
        int c = faces[i].vertices[2];
        int d = faces[i].vertices[3];

        int fi = faces[i].faceIndex;

        SDL_SetRenderDrawColor(eng->renderer,
            Object->face_colors[fi][0],
            Object->face_colors[fi][1],
            Object->face_colors[fi][2],
            255);
        float color[3] = {
            Object->face_colors[fi][0],
            Object->face_colors[fi][1],
            Object->face_colors[fi][2]
        };
        RasterizeFace(a, b, c, d, ScreenPoints, CameraPoints, color, eng);
    }
}

void DisplayObject3D(Engine * engine, Object3D * Object)
{
    float cx = 0.5f;
    float cy = 0.5f;
    float cz = 0.5f;

    SDL_SetRenderDrawColor(engine->renderer, 255,255,255,255);

    bool OffScreen = false;

    Point2D TransformedPoints[Object->vertex_count];
    MatrixPoint PreRotatedPoints[Object->vertex_count];
    MatrixPoint RotatedPoints[Object->vertex_count];

    Camera* CurrentCam = ObtainRecord();

    for (int i = 0; i < Object->vertex_count; i++)
    {
        MatrixPoint Point = {
            .x = Object->vertices[i][0] - cx,
            .y = Object->vertices[i][1] - cy,
            .z = Object->vertices[i][2] - cz
        };
            
        TransformMatrix_X(&Point, Object->Orientation[0]); // Change both to Temp Angle for demo
        TransformMatrix_Y(&Point, Object->Orientation[1]);
        
        // WORLD POINT
        Point.x += Object->Position[0]; 
        Point.y += Object->Position[1];
        Point.z += Object->Position[2];

        // CAM_REL
        Point.x -= CurrentCam->position[0];
        Point.y -= CurrentCam->position[1];
        Point.z -= CurrentCam->position[2] - 3; // 3 is engine z offset
         
        TransformMatrix_X(&Point, -CurrentCam->orientation[1]); 
        TransformMatrix_Y(&Point, -CurrentCam->orientation[0]);  

        PreRotatedPoints[i] = Point;
        TransformToProjection(&Point, FOV); // Transformed , Temp FOV
        RotatedPoints[i] = Point;

        if (Point.z >= 1)
        {
            OffScreen = true;
        } else {
            Point2D CPoint = ToCartesianPoint(&Point);
            //printf("%f, %f \n", CPoint.x, CPoint.y);
            TransformedPoints[i] = CPoint; 
        } 
    }

    if (!OffScreen)
    {
        if (EdgeLineEnabled)
        {
            for (int i = 0; i < sizeof(Object->edges) / sizeof(Object->edges[0]); i++)
            {
                int a = Object->edges[i][0]; int b = Object->edges[i][1];
                SDL_RenderDrawLine(engine->renderer, TransformedPoints[a].x, TransformedPoints[a].y,TransformedPoints[b].x,TransformedPoints[b].y);
            }
        }


        // Face Functions

        ObjectSimpleFaceHandle(Object, PreRotatedPoints, TransformedPoints, engine);

    }
}