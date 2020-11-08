#ifndef POINT_H
#define POINT_H

#include <iostream>

struct Point
{
    double _x;
    double _y;
    double _z;

    Point() : _x(0), _y(0), _z(0) {}

    Point(float x_, float y_, float z_) : _x(x_), _y(y_), _z(z_) {}

    bool operator==(const Point &o) const;
};

Point operator-(const Point &p1, const Point &p2);

Point operator*(const Point &p1, const Point &p2);
Point operator*(float k, const Point &p);
Point operator*(const Point &p, float k);

Point operator/(const Point &p1, const Point &p2);
Point operator/(const Point &p, float k);
// Point operator/(float k, const Point &p);

Point operator+(const Point &p1, const Point &p2);

float norm(const Point &p);
Point normalize(const Point &p);
float dot(const Point &p1, const Point &p2);
Point cross(const Point &p1, const Point &p2);

std::ostream &operator<<(std::ostream &o, const Point &p);
#endif
