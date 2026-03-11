#ifndef OBJECT_H
#define OBJECT_H

typedef struct Object3D {
    float position[3];
    float rotation[3];

    float (*vertices)[3];
    int vertex_count;

    int (*faces)[4];
    int face_count;

    int (*edges)[2];
    int edge_count;

    int (*face_colors)[3];
    int face_color_count;

    float Position[3];
    float Orientation[2];
} Object3D;

typedef struct Object2D {
    float position[2];
    
} Object2D;


// Func
Object3D* CreateObject3D(
    float vertices[][3], int vertex_count,
    int faces[][4], int face_count,
    int face_colors[][3], int face_color_count);
void FreeObjects3D();

#endif