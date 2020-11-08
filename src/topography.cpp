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
