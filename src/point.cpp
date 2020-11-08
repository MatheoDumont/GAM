#include "point.h"
#include <cmath>

bool Point::operator==(const Point &p) const
{
    return (_x == p._x) && (_y == p._y) && (_z == p._z);
}

std::ostream &operator<<(std::ostream &o, const Point &p)
{
    o << "(" << p._x << ", " << p._y << ", " << p._z << ")";
    return o;
}

Point operator-(const Point &p1, const Point &p2)
{
    return Point(p1._x - p2._x, p1._y - p2._y, p1._z - p2._z);
}

Point operator*(const Point &p1, const Point &p2)
{
    return Point(p1._x * p2._x, p1._y * p2._y, p1._z * p2._z);
}

Point operator*(const Point &p, float k)
{
    return Point(p._x * k, p._y * k, p._z * k);
}

Point operator*(float k, const Point &p)
{
    return p * k;
}

Point operator/(const Point &p1, const Point &p2)
{
    return Point(p1._x / p2._x, p1._y / p2._y, p1._z / p2._z);
}

Point operator/(const Point &p, float k)
{
    return Point(p._x / k, p._y / k, p._z / k);
}

Point operator+(const Point &p1, const Point &p2)
{
    return Point(p1._x + p2._x, p1._y + p2._y, p1._z + p2._z);
}

float norm(const Point &p)
{
    return sqrt((p._x * p._x) +
                (p._y * p._y) +
                (p._z * p._z));
}

Point normalize(const Point &p)
{
    return p / norm(p);
}

float dot(const Point &p1, const Point &p2)
{
    return (p1._x * p2._x) + (p1._y * p2._y) + (p1._z * p2._z);
}

Point cross(const Point &p1, const Point &p2)
{
    Point p;
    p._x = (p1._y * p2._z) - (p1._z * p2._y);
    p._y = (p1._z * p2._x) - (p1._x * p2._z);
    p._z = (p1._x * p2._y) - (p1._y * p2._x);
    return p;
}
