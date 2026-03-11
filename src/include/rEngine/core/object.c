#include <core/object.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

Object3D * Objects3DLoaded = NULL;
int Obj3DCount = 0;

//Object2D Objects2DLoaded;

typedef struct { int (*edges)[2]; int edge_count; } EdgeReturn;

EdgeReturn * CreateEdges(int faces[][4], int face_count)
{   
    int edges[face_count * 4][2];
    int edge_count = 0;

    for (int face = 0; face < face_count; face++)
    {
        int cFace[4];
        memcpy(cFace, faces[face], sizeof(int[4]));
        for (int i = 0; i < 4; i++)
        {   
            int edge[2] = {};

            edge[0] = cFace[i];
            edge[1] = cFace[(i + 1) % 4];
            
            
            memcpy(edges[edge_count], edge, sizeof(int[2]));
            edge_count++; 
        }
    }

    EdgeReturn *EdgeFull = malloc(sizeof(EdgeReturn));

    EdgeFull->edges = malloc(sizeof(int) * (face_count * 4) * 2);
    EdgeFull->edge_count = edge_count;

    memcpy(EdgeFull->edges, edges, sizeof(int[(face_count * 4) * 2]));

    return EdgeFull;
}

Object3D* CreateObject3D(
    float vertices[][3], int vertex_count,
    int faces[][4], int face_count,
    int face_colors[][3], int face_color_count)
{
    Object3D object;

    object.vertex_count = vertex_count;
    object.vertices = malloc(sizeof(float[3]) * vertex_count);
    for(int i = 0; i < vertex_count; i++)
        for(int j = 0; j < 3; j++)
            object.vertices[i][j] = vertices[i][j];

    object.face_count = face_count;
    object.faces = malloc(sizeof(int[4]) * face_count);
    for(int i = 0; i < face_count; i++)
        for(int j = 0; j < 4; j++)
            object.faces[i][j] = faces[i][j];

    object.face_color_count = face_color_count;
    object.face_colors = malloc(sizeof(int[3]) * face_color_count);
    for(int i = 0; i < face_color_count; i++)
        for(int j = 0; j < 3; j++)
            object.face_colors[i][j] = face_colors[i][j];

        
    EdgeReturn *edge_data = CreateEdges(faces, face_count);
    int edge_count = edge_data->edge_count;
    int (*edges)[2] = edge_data->edges;

    object.edge_count = edge_count;
    object.edges = malloc(sizeof(int[2]) * edge_count);
    for(int i = 0; i < edge_count; i++)
        for(int j = 0; j < 2; j++)
            object.edges[i][j] = edges[i][j];

    Obj3DCount++;
    Objects3DLoaded = realloc(Objects3DLoaded, Obj3DCount * sizeof(Object3D));
    Objects3DLoaded[Obj3DCount - 1] = object;

    free(edge_data->edges);
    free(edge_data);

    return &Objects3DLoaded[Obj3DCount - 1];
}


void FreeObjects3D()
{
    for (int i = 0; i < Obj3DCount; i++)
    {
        free(Objects3DLoaded[i].vertices);
        free(Objects3DLoaded[i].faces);
        free(Objects3DLoaded[i].edges);
        free(Objects3DLoaded[i].face_colors);
    }

    free(Objects3DLoaded);
    Objects3DLoaded = NULL;
    Obj3DCount = 0;
}