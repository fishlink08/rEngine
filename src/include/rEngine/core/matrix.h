#ifndef MATRIX_H
#define MATRIX_H

typedef struct {
    float x;
    float y;
    float z;
} MatrixPoint;

typedef struct {float x; float y;} Point2D;

void SetupPlane(float farPlane, float nearPlane, float AspectRatio, int WS[2]);

MatrixPoint* TransformMatrix_Y(MatrixPoint* p, float angle);
MatrixPoint* TransformMatrix_X(MatrixPoint* p, float angle);
MatrixPoint* TransformToProjection(MatrixPoint* p, float FOV);

Point2D ToCartesianPoint(MatrixPoint* p);
double DotProduct(double v[], double u[], int n);

#endif