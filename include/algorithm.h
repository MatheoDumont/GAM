#ifndef ALGORITHM_H
#define ALGORITHM_H

#include "mesh.h"
#include <vector>
#include "point.h"
#include <cmath>
#include <algorithm>

class Algorithm
{
private:
    Algorithm() {}

public:
    static std::vector<Point> vertices_laplacien(Mesh &m);
    static Point laplacien(Mesh &m, Sommet &s);

    static float aire_triangle(const Point &a, const Point &b, const Point &c);
    static float aire_triangle(const std::vector<Sommet> &v_s, const Triangle &t);

    static Point normal_from_laplacien(const Point &laplacien);

    static Point curvature_from_laplacien(const Point &laplacien);

    // https://stackoverflow.com/questions/3738384/stable-cotangent
    // static float cotangente(float theta);
};
#endif