#ifndef TOPOGRAPHY_H
#define TOPOGRAPHY_H

#include "point.h"
#include <vector>
#include <random>

static std::random_device rd;  //Will be used to obtain a seed for the random number engine
static std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
static std::uniform_real_distribution<> dis(0.f, 1.0f);

struct Sommet
{
    Point p;
    // indice d'un triangle ayant un sommet au point p
    int t = -1;

    Sommet(double _x, double _y, double _z, int _t) : p(Point(_x, _y, _z)), t(_t) {}

    Sommet(double _x, double _y, double _z) : p(Point(_x, _y, _z)), t(-1) {}

    Sommet(Point _p) : p(_p), t(-1) {}

    bool operator==(const Sommet &o) const { return this->p == o.p; }
};
std::ostream &operator<<(std::ostream &o, const Sommet &s);

struct Triangle
{
private:
    bool is_barycenter_computed = false;
    Point _barycenter;
    

    void compute_barycenter(const std::vector<Sommet> &ss);

public:
    /*
    * Sommet Definit dans le sens trigonometrique, 
    * donc :
    * s1 : 0
    * s2 : 1
    * s3 : 2
    */
    int s[3];
    // int s1;
    // int s2;
    // int s3;

    /*
    * adj3 represente le sommet s a l'indice id dans le triangle represente en face du cote 
    * qui a pour sommet Liaison
    * Liaison  |   s    | id
    * -----------------------
    * s1 -> s2 |   s3   | 2
    * s2 -> s3 |   s1   | 0
    * s3 -> s1 |   s2   | 1
    */
    int adj3[3] = {-1, -1, -1};

    bool is_inf = false;

    Point color = Point(dis(gen), dis(gen), dis(gen));

    Triangle(int _s1, int _s2, int _s3) : s{_s1, _s2, _s3}, is_inf(false) {}

    Triangle(int _s1, int _s2, int _s3, bool _is_inf) : s{_s1, _s2, _s3}, is_inf(_is_inf) {}

    void update_3adj(int t1, int t2, int t3);

    int which_vertex(std::vector<Sommet> &ss, Sommet &o);
    int which_vertex(int idx_sommet);

    int which_adjacence(int idx_triangle);

    Point get_barycenter(const std::vector<Sommet> &ss);
};
std::ostream &operator<<(std::ostream &o, const Triangle &t);

#endif