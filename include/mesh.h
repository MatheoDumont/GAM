#ifndef MESH_H
#define MESH_H

#include <QGLWidget>
#include <vector>
#include "point.h"
#include "topography.h"
#include <random>
#include <queue>
#include <unordered_set>
#include <fstream>
#include <iostream>
#include <string>
#include <map>
#include <cassert>
#include <chrono>
#include <thread>

//** TP : TO MODIFY

class Mesh
{
    static const int IDX_INF_POINT = 0;

    // (Q ou STL)Vector of vertices
    // (Q ou STL)Vector of faces
    // Those who do not know about STL Vectors should have a look at cplusplus.com examples
public:
    std::vector<Point> laplaciens;
    std::vector<Sommet> sommets;
    std::vector<Triangle> triangles;

    Mesh(); // Constructors automatically called to initialize a Mesh (default strategy)
    //~Mesh(); // Destructor automatically called before a Mesh is destroyed (default strategy)

    // BASE FUNCTIONS
    void drawMesh();

    void drawMeshWireFrame();

    void tetrahedron();

    void make_adjacence();

    void load_from_file(std::string);

    // LAPLACIENS
    void compute_laplaciens();

    // TRIANGULATIONS
    void make_inf_point();

    /*
     * Constructions des triangles reliees au point infini, make_inf_point doit etre appeler avant.
     * Appelle a la fin make_adjacence().
     * L'adjacence des triangles doit etre deja faite.
     */
    void make_infinite_triangles();

    void triangle_split(int idx_triangle, int idx_sommet, bool is_inf = false);

    /*
     * regarde si le point dans t1 qui fait face a t2 peut voir les 2 aretes de t2, 
     * ce qui determine si l'aretes du centre est flippable.
     * On retourne:
     * true si flippable
     * false sinon
     */
    bool is_flippable(int idx_t1, int idx_t2);

    /*
     * On flip l'edge entre 2 triangles dans le sens trigonometrique,
     * C'est-a-dire que :
     * 
     * t1 | t2 
     * devienne
     * t2
     * -
     * t1
     * puis
     * t2 | t1
     * et enfin
     * t1
     * -
     * t2
     */
    void edge_flip(int idx_t1, int idx_t2);

    /*
     * Predicat d'orientation entre 3 points
     * t1 = p
     * t2 = q
     * t3 = r 
     * p pts pivot, appartenant au segment q - p
     * r pts dont on veut connaitre l'orientation
     * return 1 si oriente counter-clockwise
     * return -1 si oriente clock-wise
     * return 0 si aligne
     */
    int orientation(const Point &p, const Point &q, const Point &r) const;

    int orientation(int p, int q, int r) const;

    /*
     * Predicat pour savoir si un point est a l'interieur d'un triangle
     * return 0 si sur la bordure
     * return 1 si a l'interieur
     * return -1 si a l'exterieur
     */
    int in_triangle(int t, const Point &p) const;

    /*
     * Predicat qui indique si s est en dessous ou au dessus. du plan definit par le
     * triangle represente par les points p q et r.
     * Remonte les points sur un paraboloid centre en p.
     * return 0 si sur la bordure
     * return 1 si a l'interieur
     * return -1 si a l'exterieur
     */
    int in_circumscribed_cercle(int i_p, int i_q, int i_r, const Point &s) const;

    /*
     * Methode qui retourne le triangle dans lequel est inclut le point p.
     * La strategie utilisee est : visibility march.
     * return l'indice du triangle.
     * -1 si pas dans l'enveloppe convex.
     */
    int locate_triangle_fourmis(int idx_p) const;

    int locate_triangle_iterative(int idx_p) const;

    void add_delaunay_point(Point p);

    void load_and_add_delaunay(std::string file);

    typedef std::vector<Sommet>::iterator Iterator_on_vertices;
    typedef std::vector<Triangle>::iterator Iterator_on_faces;

    class Circulator_on_faces
        : public std::iterator<std::forward_iterator_tag, Triangle, int, const Triangle *, Triangle>
    {

    public:
        std::vector<Triangle> &faces;
        Sommet &s;
        int idx_sommet;
        int idx, idx_start_face;
        int _get_idx_sommet() { return this->idx_sommet; }

        Circulator_on_faces(std::vector<Triangle> &_faces, Sommet &_s, int _idx_sommet, int _start_idx_face)
            : faces(_faces),
              s(_s), idx_sommet(_idx_sommet),
              idx(_start_idx_face),
              idx_start_face(_start_idx_face) {}

        Circulator_on_faces &operator++()
        {
            int idx_sommet_on_current_face = faces[idx].which_vertex(idx_sommet);

            int sommet_suivant = (idx_sommet_on_current_face + 1) % 3;

            idx = faces[idx].adj3[sommet_suivant];

            return *this;
        }

        bool operator==(Circulator_on_faces other) const { return idx == other.idx; }

        bool operator!=(Circulator_on_faces other) const { return !(*this == other); }

        Triangle &operator*() { return faces[idx]; }

        Triangle *operator->() { return &faces[idx]; }
    };

    class Circulator_on_vertices
        : public std::iterator<std::forward_iterator_tag, Sommet, int, const Sommet *, Sommet>
    {
        Mesh::Circulator_on_faces circulator;
        std::vector<Sommet> &ss;
        int idx_start_sommet;
        int idx_current_sommet_relative_to_face;

    public:
        Circulator_on_vertices(Mesh::Circulator_on_faces _circ, std::vector<Sommet> &_ss,
                               int _idx_start_sommet, int _idx_current_sommet_relative_to_face)
            : circulator(_circ), ss(_ss), idx_start_sommet(_idx_start_sommet),
              idx_current_sommet_relative_to_face(_idx_current_sommet_relative_to_face) {}

        Circulator_on_vertices &operator++()
        {
            if (circulator->s[idx_current_sommet_relative_to_face] == idx_start_sommet)
                return *this;

            ++this->circulator;
            int idx_sommet_on_current_face = this->circulator->which_vertex(idx_start_sommet);

            if (idx_sommet_on_current_face + 1 > 2)
                idx_current_sommet_relative_to_face = 0;
            else
                idx_current_sommet_relative_to_face = idx_sommet_on_current_face + 1;

            return *this;
        }

        bool operator==(Circulator_on_vertices other)
        {
            return ss[circulator->s[idx_current_sommet_relative_to_face]] ==
                   ss[other.circulator->s[other.idx_current_sommet_relative_to_face]];
        }

        bool operator!=(Circulator_on_vertices other) { return !(*this == other); }

        Sommet &operator*() { return ss[circulator->s[idx_current_sommet_relative_to_face]]; }

        Sommet *operator->() { return &ss[circulator->s[idx_current_sommet_relative_to_face]]; }
    };

    Iterator_on_vertices vertices_begin() { return this->sommets.begin(); }

    Iterator_on_vertices vertices_end() { return this->sommets.end(); }

    Iterator_on_faces faces_begin() { return this->triangles.begin(); }

    Iterator_on_faces faces_end() { return this->triangles.end(); }

    Circulator_on_faces incident_faces(Sommet &s)
    {

        int idx_start_face = s.t;
        Triangle &t = triangles[idx_start_face];
        int idx_relative_t = t.which_vertex(sommets, s);

        if (idx_relative_t == -1)
            std::cout << "!! WARNING !! idx_relative_t == -1 line 160 mesh.h" << std::endl;

        int idx_sommet = t.s[idx_relative_t];

        return Circulator_on_faces(triangles, s, idx_sommet, idx_start_face);
    }

    Circulator_on_vertices adjacent_vertices(Sommet &s)
    {
        Mesh::Circulator_on_faces circ = incident_faces(s);
        int idx_start_sommet = circ._get_idx_sommet();
        int idx_relative_s = circ->which_vertex(idx_start_sommet);

        int idx_relative_start_sommet;
        if (idx_relative_s + 1 > 2)
            idx_relative_start_sommet = 0;
        else
            idx_relative_start_sommet = idx_relative_s + 1;

        return Circulator_on_vertices(circ, sommets, idx_start_sommet, idx_relative_start_sommet);
    }
};

class GeometricWorld //Generally used to create a singleton instance
{
    QVector<Point> _bBox; // Bounding box
public:
    GeometricWorld();

    void draw();

    void drawWireFrame();

    // ** TP Can be extended with further elements;
    Mesh _mesh;
};

#endif // MESH_H
