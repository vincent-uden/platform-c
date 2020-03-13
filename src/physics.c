#include "../headers/physics.h"

int between(double lowerBound, double upperBound, double value) {
    return value <= upperBound && value >= lowerBound;
}

/* Vector */
double
VectorMag(Vector v){
    return sqrt(pow(v.x, 2) + pow(v.y, 2));
}

/* TODO: Test this function */
double VectorAngle(Vector v) {
    /* Avoid division by zero */
    if ( v.x == 0 ) {
        if ( v.y > 0 ) {
            return HALF_PI;
        } else {
            return M_PI + HALF_PI;
        }
    } else {
        double angle = atan(v.y / v.x);
        if ( v.x < 0 ) {
            if ( v.y < 0 ) {
                return angle + M_PI;
            } else {
                return M_PI - angle * -1;
            }
        } else {
            if ( v.y < 0 ) {
                return angle + M_PI * 2;
            } else {
                return angle;
            }
        }
    }
}

double VectorScalar(Vector u, Vector v) {
    if ( VectorMag(u) == 0 || VectorMag(v) == 0 ) 
        return 0;
    
    double absu = VectorMag(u);
    double absv = VectorMag(v);
    double angle = VectorAngle(u) - VectorAngle(v);
    if ( angle < 0 )
        angle = 2* M_PI + angle;

    return fabs(absu * absv * cos(angle));
}

double VectorProject(Vector u, Vector v) {
    /* Returns the length of u projected on v */
    if ( VectorMag(u) == 0 || VectorMag(v) == 0 ) 
        return 0;
    
    double absu = VectorMag(u);
    double angle = VectorAngle(u) - VectorAngle(v);
    if ( angle < 0 )
        angle = 2* M_PI + angle;

    return fabs(absu * cos(angle));
}

Vector
VectorAdd(Vector v1, Vector v2){
    return (Vector) { v1.x + v2.x, v1.y + v2.y };
}

Vector
VectorSub(Vector v1, Vector v2){
    return (Vector) { v1.x - v2.x, v1.y - v2.y };
}

Vector
VectorMul(Vector v, double scalar){
    return (Vector) { v.x * scalar, v.y * scalar };
}

Vector
VectorDiv(Vector v, double denominator){
    return (Vector) { v.x / denominator, v.y / denominator };
}

Vector 
VectorHalfSize(Vector v) {
    return VectorDiv(v, 2);
}

Vector 
VectorFromPolar(double mag, double angle) {
    return (Vector) { mag * cos(angle), mag * sin(angle) };
}

Vector 
VectorRotateOnPoint(Vector v, Vector point, double angle) {
    VectorSubIp(&v, point);
    double mag = VectorMag(v);
    double currAngle = acos(v.x / mag);
    /* Account for the two answers of acos */
    if ( v.y < 0 )
        currAngle *= -1;
    double newAngle = currAngle + angle;
    v.x = mag * cos(newAngle);
    v.y = mag * sin(newAngle);
    VectorAddIp(&v, point);
    return v;
}

void
VectorAddIp(Vector* v1, Vector v2){
    v1->x += v2.x;
    v1->y += v2.y;
}

void
VectorSubIp(Vector* v1, Vector v2){
    v1->x -= v2.x;
    v1->y -= v2.y;
}

void
VectorMulIp(Vector* v, double scalar){
    v->x *= scalar;
    v->y *= scalar;
}

void
VectorDivIp(Vector* v, double denominator){
    v->x /= denominator;
    v->y /= denominator;
}

void 
VectorPrint(Vector v) {
    printf("x: %f y: %f\n", v.x, v.y);
}

int 
VectorEq(Vector v1, Vector v2) {
    return v1.x == v2.x && v1.y == v2.y;
}

Vector isColliding(ColliderRect c1, ColliderRect c2) {
    Vector push;
    // Check x-overlap
    if ( c1.pos.x < c2.pos.x && c1.pos.x + c1.size.x > c2.pos.x ) {
        push.x = c1.pos.x - c2.pos.x + c1.size.x;
    }
    if ( c1.pos.x < c2.pos.x + c2.size.x && c1.pos.x + c1.size.x > c2.pos.x + c2.size.x ) {
        push.x = c2.pos.x - c1.pos.x + c2.size.x;
    }
    // Check y-overlap 
    if ( c1.pos.y < c2.pos.y && c1.pos.y + c1.size.y > c2.pos.y ) {
        push.y = c1.pos.y - c2.pos.y + c1.size.y;
    }
    if ( c1.pos.y < c2.pos.y + c2.size.y && c1.pos.y + c1.size.y > c2.pos.y + c2.size.y ) {
        push.y = c2.pos.y - c1.pos.y + c2.size.y;
    }
    return push;
}

