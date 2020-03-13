#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef M_PI
#   define M_PI     3.14159265358979323846
#endif
#ifndef HALF_PI
#   define HALF_PI  1.57079632679489661923
#endif 
int between(double lowerBound, double upperBound, double value);

typedef struct {
    double x;
    double y;
} Vector;

typedef struct {
    Vector pos;
    Vector size;
} ColliderRect;

double VectorMag(Vector v);
double VectorAngle(Vector v);
double VectorScalar(Vector u, Vector v);
double VectorProject(Vector u, Vector v);

Vector VectorAdd(Vector v1, Vector v2);
Vector VectorSub(Vector v1, Vector v2);
Vector VectorMul(Vector v, double scalar);
Vector VectorDiv(Vector v, double denominator);
Vector VectorHalfSize(Vector v);
Vector VectorFromPolar(double mag, double angle);
Vector VectorRotateOnPoint(Vector v, Vector point, double angle);

void VectorAddIp(Vector* v1, Vector v2);
void VectorSubIp(Vector* v1, Vector v2);
void VectorMulIp(Vector* v, double scalar);
void VectorDivIp(Vector* v, double denominator);
void VectorPrint(Vector v);

int VectorEq(Vector v1, Vector v2);

Vector isColliding(ColliderRect c1, ColliderRect c2);
