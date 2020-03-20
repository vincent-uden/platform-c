#include "../headers/physics.h"

/* Returns wether value is in between lower and upper bound */
int between(double lowerBound, double upperBound, double value) {
    return value <= upperBound && value >= lowerBound;
}

/* Returns the length of a given vector */
double VectorMag(Vector v){
    return sqrt(pow(v.x, 2) + pow(v.y, 2));
}

/* Returns a vectors heading (counter clockwise) */
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

/* Returns the scalar product between two vectors */
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

/* Returns the length of u when projected onto v */
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

/* Adds two vectors component-wise */
Vector VectorAdd(Vector v1, Vector v2){
    return (Vector) { v1.x + v2.x, v1.y + v2.y };
}

/* Subtracts two vectors component-wise */
Vector VectorSub(Vector v1, Vector v2){
    return (Vector) { v1.x - v2.x, v1.y - v2.y };
}

/* Multiplies a vector by a scalar amount */
Vector VectorMul(Vector v, double scalar){
    return (Vector) { v.x * scalar, v.y * scalar };
}

/* Divides a vector by a scalar amount */
Vector VectorDiv(Vector v, double denominator){
    return (Vector) { v.x / denominator, v.y / denominator };
}

/* Returns half the size of a vector, used in certain collision calculations */
Vector VectorHalfSize(Vector v) {
    return VectorDiv(v, 2);
}

/* Calculates the cartesian representation of a vector from polar coordinates */
Vector VectorFromPolar(double mag, double angle) {
    return (Vector) { mag * cos(angle), mag * sin(angle) };
}

/* Rotates a vector around a given point */
Vector VectorRotateOnPoint(Vector v, Vector point, double angle) {
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

/* Adds a vector to another in place */
void VectorAddIp(Vector* v1, Vector v2){
    v1->x += v2.x;
    v1->y += v2.y;
}

/* Subtracts a vector to another in place */
void VectorSubIp(Vector* v1, Vector v2){
    v1->x -= v2.x;
    v1->y -= v2.y;
}

/* Scale a vector in place */
void VectorMulIp(Vector* v, double scalar){
    v->x *= scalar;
    v->y *= scalar;
}

/* Divide a vector in place */
void VectorDivIp(Vector* v, double denominator){
    v->x /= denominator;
    v->y /= denominator;
}

/* Pretty prints a vector */
void VectorPrint(Vector v) {
    printf("x: %f y: %f\n", v.x, v.y);
}

/* Return if two vectors are identical */
int VectorEq(Vector v1, Vector v2) {
    return v1.x == v2.x && v1.y == v2.y;
}

/* Returns how much, if at all two ColliderRect overlap */
Vector isColliding(ColliderRect c1, ColliderRect c2) {
    Vector push;
    push.x = 0;
    push.y = 0;
    /* Check x-overlap */
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

    if ( push.x == 0 || push.y == 0 ) {
        push.x = 0;
        push.y = 0;
    } else {
        if ( push.y < 0 ) {
        puts("Push:");
        VectorPrint(push);
        }
    }
    return push;
}

