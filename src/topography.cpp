#include "topography.h"

std::ostream &operator<<(std::ostream &o, const Sommet &s)
{
    std::cout.precision(2);
    o << "point         = (" << std::fixed << s.p._x << ", " << std::fixed << s.p._y << ", " << std::fixed << s.p._z << ")" << std::endl;
    o << "trig incident =  " << s.t << std::endl;

    return o;
}

std::ostream &operator<<(std::ostream &o, const Triangle &t)
{
    o << "sommets    = (" << t.s[0] << ", " << t.s[1] << ", " << t.s[2] << ")" << std::endl;
    o << "adjacences = (" << t.adj3[0] << ", " << t.adj3[1] << ", " << t.adj3[2] << ")" << std::endl;
    o << "is_inf     =  " << t.is_inf << std::endl;

    return o;
}

void Triangle::update_3adj(int t1, int t2, int t3)
{
    adj3[0] = t1;
    adj3[1] = t2;
    adj3[2] = t3;
}

int Triangle::which_vertex(std::vector<Sommet> &ss, Sommet &o)
{
    for (int i = 0; i < 3; ++i)
    {
        if (ss[s[i]] == o)
        {
            return i;
        }
    }

    return -1;
}

int Triangle::which_vertex(int idx_sommet)
{
    for (int i = 0; i < 3; ++i)
    {
        if (s[i] == idx_sommet)
            return i;
    }
    return -1;
}

int Triangle::which_adjacence(int idx_triangle)
{
    for (int i = 0; i < 3; ++i)
    {
        if (adj3[i] == idx_triangle)
            return i;
    }
    return -1;
}

void Triangle::compute_barycenter(const std::vector<Sommet> &ss)
{
    const Point &A = ss[s[0]].p;
    const Point &B = ss[s[1]].p;
    const Point &C = ss[s[2]].p;

    // angle A == CAB
    Point AB = B - A;
    Point AC = C - A;

    Point crossed = cross(AB, AC);
    float signe = crossed._z / std::abs(crossed._z);
    float tan_A = signe * (norm(crossed) / dot(AB, AC));

    // angle B == ABC
    Point BA = A - B;
    Point BC = C - B;

    crossed = cross(BC, BA);
    signe = crossed._z / std::abs(crossed._z);
    float tan_B = signe * (norm(crossed) / dot(BC, BA));

    // angle C == BCA
    Point CB = B - C;
    Point CA = A - C;

    crossed = cross(CA, CB);
    signe = crossed._z / std::abs(crossed._z);
    float tan_C = signe * (norm(crossed) / dot(CA, CB));

    float tmp_alpha = tan_C + tan_B;
    float tmp_beta = tan_A + tan_C;
    float tmp_gamma = tan_B + tan_A;

    float alpha = tmp_alpha / (tmp_alpha + tmp_beta + tmp_gamma);
    float beta = tmp_beta / (tmp_alpha + tmp_beta + tmp_gamma);
    float gamma = tmp_gamma / (tmp_alpha + tmp_beta + tmp_gamma);

    _barycenter._x = A._x * alpha + B._x * beta + C._x * gamma;
    _barycenter._y = A._y * alpha + B._y * beta + C._y * gamma;
    _barycenter._z = A._z * alpha + B._z * beta + C._z * gamma;
}

Point Triangle::get_barycenter(const std::vector<Sommet> &ss)
{
    if (!is_barycenter_computed)
        compute_barycenter(ss);

    return _barycenter;
}
