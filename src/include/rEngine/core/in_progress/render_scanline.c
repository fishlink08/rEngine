#include "rEngine.h"

#include <stdio.h>
#include <stdbool.h>
#include <math.h>

int FOV = 90;
bool EdgeLineEnabled = false;


typedef struct {
    int vertices[4];
    float depth;
    int faceIndex;
} FaceDepth;

typedef struct {
    int x;
    int y;
} sPoint;


int DrawPoint(float x, float y, int s, Engine * eng)
{
    int CommonErrorCode = 0;
    for (int y_ = 0; y_ < s; y_++){
        for (int x_ = 0; x_ < s; x_++){
            CommonErrorCode += SDL_RenderDrawPoint(eng->renderer, x+x_, y+y_);
        }
    }

    if (CommonErrorCode != 0){ CommonErrorCode = 1; } else { CommonErrorCode = 0;}

    return CommonErrorCode;
}

void draw_vertice_connection(float point1[2], float point2[2], Engine * eng)
{
    SDL_RenderDrawLine(eng->renderer, point1[0], point1[1], point2[0], point2[1]);
}

sPoint * edge_interpolate(float point1[2], float point2[2], int * out_count)
{
    sPoint * points_in_between = NULL;
    int point_count = 0;

    float tPoint1[2];
    float tPoint2[2];

    if (point2[1] < point1[1])
    {
        tPoint1[0] = point2[0];
        tPoint1[1] = point2[1];
        
        tPoint2[0] = point1[0];
        tPoint2[1] = point1[1];
    } else {
        tPoint1[0] = point1[0];
        tPoint1[1] = point1[1];
        
        tPoint2[0] = point2[0];
        tPoint2[1] = point2[1];
    }

    float dy = tPoint2[1] - tPoint1[1];
    float dx = tPoint2[0] - tPoint1[0];

    if (fabsf(dy) < 0.0001f) return NULL;

    float slope = dx / dy;

    point_count = (int)fabsf(dy);
    if (point_count <= 0) return NULL;


    points_in_between = realloc(points_in_between, point_count * sizeof(sPoint) );

    for (int i = 0; i < point_count; i++)
    {
        float y = tPoint1[1] + i;
        float x = tPoint1[0] + (slope * i);

        sPoint point;
        point.x = (int)x;
        point.y = (int)y;

        points_in_between[i] = point;
    }

    *out_count = point_count;

    return points_in_between;
}

void fill_triangle(float point1[2], float point2[2], float point[2], Engine * eng)
{
    float pts[3][2] = {
        {point1[0], point1[1]},
        {point2[0], point2[1]},
        {point[0], point[1]}
    };
    // pts = sorted([p0, p1, p2], key=lambda p: p[1]) 
    //    p0, p1, p2 = pts
    for (int i = 0; i < 2; i++) {
        for (int j = i+1; j < 3; j++) {
            if (pts[i][1] > pts[j][1]) {
                float tmp[2] = {pts[i][0], pts[i][1]};
                pts[i][0] = pts[j][0];
                pts[i][1] = pts[j][1];
                pts[j][0] = tmp[0];
                pts[j][1] = tmp[1];
            }
        }
    }
    int left_count, right_top_count, right_bottom_count;

    sPoint* left = edge_interpolate(pts[0], pts[2], &left_count);
    sPoint* right_top = edge_interpolate(pts[0], pts[1], &right_top_count);
    sPoint* right_bottom = edge_interpolate(pts[1], pts[2], &right_bottom_count);

    if (left && right_top) {
        for (int i = 0; i < right_top_count; i++)
        {   
            if (i < left_count) {
                float p1[2] = {left[i].x, left[i].y};
                float p2[2] = {right_top[i].x, right_top[i].y};

                draw_vertice_connection(p1, p2, eng);
            }
        }
    }

    if (right_top && right_bottom) {
        for (int i = 0; i < right_bottom_count; i++)
        {
            int left_idx = i + (right_top_count > 0 ? right_top_count - 1 : 0);
            if (left_idx >= left_count) break;
            if (left_idx < left_count) {
                float p1[2] = {left[left_idx].x, left[left_idx].y};
                float p2[2] = {right_bottom[i].x, right_bottom[i].y};

                draw_vertice_connection(p1, p2, eng);
            }
        }
    }



    free(left);
    free(right_top);
    free(right_bottom);

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
    
    //SDL_RenderGeometry(eng->renderer, NULL, verts_one, 3, NULL, 0);
    //SDL_RenderGeometry(eng->renderer, NULL, verts_two, 3, NULL, 0);
    //SDL_RenderGeometry(renderer, NULL, verts, 4, indices, 6);

    //incorporate fill_triangle here for vert 1 and two
    float vert1p1[2] = {ScreenPoints[a].x, ScreenPoints[a].y};
    float vert1p2[2] = {ScreenPoints[b].x, ScreenPoints[b].y};
    float vert1p3[2] = {ScreenPoints[c].x, ScreenPoints[c].y};

    float vert2p1[2] = {ScreenPoints[a].x, ScreenPoints[a].y};
    float vert2p2[2] = {ScreenPoints[c].x, ScreenPoints[c].y};
    float vert2p3[2] = {ScreenPoints[d].x, ScreenPoints[d].y};

    fill_triangle(vert1p1, vert1p2, vert1p3, eng);
    fill_triangle(vert2p1, vert2p2, vert2p3, eng);

}


void ObjectFaceHandle(
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

        double ab[3] = {
            CameraPoints[b].x - CameraPoints[a].x,
            CameraPoints[b].y - CameraPoints[a].y,
            CameraPoints[b].z - CameraPoints[a].z
        };

        double ac[3] = {
            CameraPoints[c].x - CameraPoints[a].x,
            CameraPoints[c].y - CameraPoints[a].y,
            CameraPoints[c].z - CameraPoints[a].z
        };

        double normal[3] = {
            ab[1]*ac[2] - ab[2]*ac[1],
            ab[2]*ac[0] - ab[0]*ac[2],
            ab[0]*ac[1] - ab[1]*ac[0]
        };

        double center[3] = {
            (CameraPoints[a].x + CameraPoints[b].x + CameraPoints[c].x + CameraPoints[d].x) / 4.0,
            (CameraPoints[a].y + CameraPoints[b].y + CameraPoints[c].y + CameraPoints[d].y) / 4.0,
            (CameraPoints[a].z + CameraPoints[b].z + CameraPoints[c].z + CameraPoints[d].z) / 4.0
        };

        if (DotProduct(normal, center, 3) > 0) {
            normal[0] *= -1;
            normal[1] *= -1;
            normal[2] *= -1;
        }

        // view vector (face → camera)
        double view[3] = {
            -center[0],
            -center[1],
            -center[2]
        };

        double dot = DotProduct(normal, view, 3);

        if (dot <= 0) continue;
        if (
            CameraPoints[a].z <= 0.01 ||
            CameraPoints[b].z <= 0.01 ||
            CameraPoints[c].z <= 0.01 ||
            CameraPoints[d].z <= 0.01
        ) {
            continue;
        }

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
        Point.z -= CurrentCam->position[2]; 
         
        TransformMatrix_X(&Point, -CurrentCam->orientation[1]); 
        TransformMatrix_Y(&Point, -CurrentCam->orientation[0]);  

        PreRotatedPoints[i] = Point;
        TransformToProjection(&Point, FOV); // Transformed , Temp FOV
        RotatedPoints[i] = Point;

        Point2D CPoint = ToCartesianPoint(&Point);
        //printf("%f, %f \n", CPoint.x, CPoint.y);
        TransformedPoints[i] = CPoint; 

    }


    if (EdgeLineEnabled)
    {
        for (int i = 0; i < sizeof(Object->edges) / sizeof(Object->edges[0]); i++)
        {
            int a = Object->edges[i][0]; int b = Object->edges[i][1];
            SDL_RenderDrawLine(engine->renderer, TransformedPoints[a].x, TransformedPoints[a].y,TransformedPoints[b].x,TransformedPoints[b].y);
        }
    }

    ObjectFaceHandle(Object, PreRotatedPoints, TransformedPoints, engine);

}