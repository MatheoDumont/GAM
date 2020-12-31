#include "mesh.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "laplacien.h"
#include <cassert>
#include <chrono>
#include <thread>

std::vector<std::string> split(std::string);

GeometricWorld::GeometricWorld()
{
    double width = 0.5, depth = 0.6, height = 0.8;
    _bBox.push_back(Point(-0.5 * width, -0.5 * depth, -0.5 * height)); //0
    _bBox.push_back(Point(-0.5 * width, 0.5 * depth, -0.5 * height));  // 1
    _bBox.push_back(Point(0.5 * width, -0.5 * depth, -0.5 * height));  // 2
    _bBox.push_back(Point(-0.5 * width, -0.5 * depth, 0.5 * height));  // 3

    // this->_mesh = Mesh();
}

// The following functions could be displaced into a module OpenGLDisplayGeometricWorld that would include mesh.h

// Draw a Point
void glPointDraw(const Point &p)
{
    glVertex3f(p._x, p._y, p._z);
}

//Example with a bBox
void GeometricWorld::draw()
{
    // glColor3d(1, 0, 0);
    // glBegin(GL_TRIANGLES);
    // glPointDraw(_bBox[0]);
    // glPointDraw(_bBox[1]);
    // glPointDraw(_bBox[2]);
    // glEnd();

    // glColor3d(0, 1, 0);
    // glBegin(GL_TRIANGLES);
    // glPointDraw(_bBox[0]);
    // glPointDraw(_bBox[2]);
    // glPointDraw(_bBox[3]);
    // glEnd();

    // glColor3d(0, 0, 1);
    // glBegin(GL_TRIANGLES);
    // glPointDraw(_bBox[0]);
    // glPointDraw(_bBox[3]);
    // glPointDraw(_bBox[1]);
    // glEnd();
    glColor3d(0, 1, 0);
    glBegin(GL_LINE_STRIP);
    glPointDraw(_bBox[0]);
    glPointDraw(_bBox[1]);
    glEnd();
    glColor3d(0, 0, 1);
    glBegin(GL_LINE_STRIP);
    glPointDraw(_bBox[0]);
    glPointDraw(_bBox[2]);
    glEnd();
    glColor3d(1, 0, 0);
    glBegin(GL_LINE_STRIP);
    glPointDraw(_bBox[0]);
    glPointDraw(_bBox[3]);
    glEnd();
    this->_mesh.drawMesh();
}

//Example with a wireframe bBox
void GeometricWorld::drawWireFrame()
{
    glColor3d(0, 1, 0);
    glBegin(GL_LINE_STRIP);
    glPointDraw(_bBox[0]);
    glPointDraw(_bBox[1]);
    glEnd();
    glColor3d(0, 0, 1);
    glBegin(GL_LINE_STRIP);
    glPointDraw(_bBox[0]);
    glPointDraw(_bBox[2]);
    glEnd();
    glColor3d(1, 0, 0);
    glBegin(GL_LINE_STRIP);
    glPointDraw(_bBox[0]);
    glPointDraw(_bBox[3]);
    glEnd();
    this->_mesh.drawMeshWireFrame();
    // this->_mesh.drawMesh();
}

void GeometricWorld::drawVoronoi()
{
    this->_mesh.drawMeshWireFrame();
    this->_mesh.draw_voronoi();
}

void GeometricWorld::dawContourCrust()
{
    _mesh.display_contour_crust();
}

void GeometricWorld::drawCourbure()
{
    _mesh.drawLaplacien();
}

Mesh::Mesh() : laplaciens(), sommets(), triangles()
{
    // Point infini
    this->make_inf_point();
    boite_englobante(1);
    // ruppert();
    // crust();

    // load_from_file("data/bunny.off");

    // this->tetrahedron();
    // load_from_file("data/queen.off");
    // load_from_file("data/cube.off");
    // this->compute_laplaciens();
    // this->edge_flip(0, 1);

    // sommets.emplace_back(0.25, 0.25, 0.f);
    // this->triangle_split(0, 4);

    // this->edge_flip(0, 1);
    // add_delaunay_point(Point(0.25, 0.25, 0.f));
    // load_and_add_delaunay("data/franke4.off");
    // Point R = Point(0.4, 0.4, 0.f);
    // Point L = Point(0.5, -1., 0.);
    // Point D = Point(-1., -1., 0.);
    // add_delaunay_point(R);
    // add_delaunay_point(L);
    // add_delaunay_point(D);
}

void Mesh::drawMesh()
{
    for (unsigned int i = 0; i < this->triangles.size(); ++i)
    {
        Triangle &t = triangles[i];

        glBegin(GL_TRIANGLES);

        if (triangles[i].is_inf)
        {
            glColor3f(1., 1., 1.);
        }
        else
        {
            glColor3f(1., 0., 0.);
        }
        // glColor3f(t.color._x, t.color._y, t.color._z);

        glPointDraw(sommets[t.s[0]].p);

        glPointDraw(sommets[t.s[1]].p);

        glPointDraw(sommets[t.s[2]].p);

        glEnd();
    }
}

void Mesh::drawLaplacien()
{

    for (unsigned int i = 0; i < this->triangles.size(); ++i)
    {
        Triangle &t = triangles[i];

        glBegin(GL_TRIANGLES);
        float k = 3.0f;
        float l = norm(laplaciens[t.s[0]]) * k;
        // l = std::pow(l, 0.2);
        float x = (l - min_curvature) / (max_curvature - min_curvature);
        glColor3f(x, 0., 1.0 - x);
        glPointDraw(sommets[t.s[0]].p);

        l = norm(laplaciens[t.s[1]]) * k;
        x = (l - min_curvature) / (max_curvature - min_curvature);
        glColor3f(x, 0., 1.0 - x);
        glPointDraw(sommets[t.s[1]].p);

        l = norm(laplaciens[t.s[2]]) * k;
        x = (l - min_curvature) / (max_curvature - min_curvature);
        glColor3f(x, 0., 1.0 - x);
        glPointDraw(sommets[t.s[2]].p);

        glEnd();
    }
}

void Mesh::drawCircle(int i_t)
{
    const Triangle &t = triangles[i_t];

    Point centre = triangles[i_t].get_barycenter(sommets);
    double radius = norm(centre - sommets[t.s[0]].p);
    float step = 6.28 / 1000;
    Point border(radius, 0., 0.);

    glBegin(GL_LINE_STRIP);
    for (float angle = 0.f; angle < 6.28; angle += step)
    {
        Point p;
        p._x = border._x * cos(step) - border._y * sin(step);
        p._y = border._x * sin(step) + border._y * cos(step);
        border = p;
        glVertex3d(border._x + centre._x, border._y + centre._y, 0.f);
    }
    glEnd();
}

void Mesh::drawMeshWireFrame()
{
    for (unsigned int i = 0; i < this->triangles.size(); ++i)
    {
        glColor3f(1., 1., 1.);

        if (triangles[i].is_inf)
        {
            glColor3f(1., 1., 0.);
        }
        // else
        // {
        //     drawCircle(i);
        // }

        glBegin(GL_LINE_STRIP);
        glPointDraw(sommets[triangles[i].s[0]].p);
        glPointDraw(sommets[triangles[i].s[1]].p);
        glEnd();

        glBegin(GL_LINE_STRIP);
        glPointDraw(sommets[triangles[i].s[1]].p);
        glPointDraw(sommets[triangles[i].s[2]].p);
        glEnd();

        glBegin(GL_LINE_STRIP);
        glPointDraw(sommets[triangles[i].s[2]].p);
        glPointDraw(sommets[triangles[i].s[0]].p);
        glEnd();
    }
}

void Mesh::draw_voronoi()
{
    for (int i = 0; i < triangles.size(); ++i)
    {
        float k = 10;
        const Triangle &t = triangles[i];
        Point p = triangles[i].get_barycenter(sommets);

        glColor3f(1., 0., 0.);

        if (!t.is_inf)
        {
            for (int j = 0; j < 3; ++j)
            {
                bool draw = false;
                glBegin(GL_LINE_STRIP);
                glPointDraw(p);

                Point pp;
                if (!triangles[t.adj3[j]].is_inf)
                    pp = triangles[t.adj3[j]].get_barycenter(sommets);
                else
                {
                    // prendre comme direction la normale a la mediatrice du segment viser
                    Point mediatrice = (sommets[t.s[(j + 1) % 3]].p + sommets[t.s[(j + 2) % 3]].p) / 2.f;
                    Point dir = normalize(mediatrice - p);
                    if (in_triangle(i, p) == 1)
                        pp = p + (k * dir);
                    else
                        pp = p + (k * (Point() - dir));
                }
                glPointDraw(pp);
                glEnd();
            }
        }
    }
}

void Mesh::tetrahedron()
{
    int v1, v2, v3, v4;
    v1 = 0;
    v2 = 1;
    v3 = 2;
    v4 = 3;

    sommets.emplace_back(0.f, 0.f, 0.f);
    sommets.emplace_back(1.f, 0.f, 0.f);
    sommets.emplace_back(0.5, 1.f, 0.5);
    sommets.emplace_back(0.5, 0.f, 1.f);

    triangles.emplace_back(v1, v2, v3);
    triangles.emplace_back(v1, v3, v4);
    triangles.emplace_back(v2, v1, v4);
    triangles.emplace_back(v3, v2, v4);

    this->make_adjacence();
}

std::vector<std::string> split(std::string s)
{
    std::vector<std::string> vec;
    std::string tmp;

    for (size_t i = 0; i < s.size(); i++)
    {
        if (s[i] == ' ')
        {
            vec.push_back(tmp);
            tmp.clear();
        }
        else
        {
            tmp += s[i];
        }
    }
    vec.push_back(tmp);

    return vec;
}

std::map<std::pair<int, int>, std::pair<int, int>>::iterator finds(std::map<std::pair<int, int>, std::pair<int, int>> &m, std::pair<int, int> key)
{
    std::map<std::pair<int, int>, std::pair<int, int>>::iterator ite = m.find(key);
    if (ite != m.end())
    {
        return ite;
    }

    ite = m.find({key.second, key.first});
    if (ite != m.end())
    {
        return ite;
    }
    return m.end();
}

void Mesh::make_adjacence()
{
    std::map<std::pair<int, int>, std::pair<int, int>> connected_faces;
    std::map<std::pair<int, int>, std::pair<int, int>>::iterator ite;

    for (int i = 0; i < triangles.size(); ++i)
    // for (int i = 0; i < faces; ++i)
    {

        for (int j = 0; j < 3; ++j)
            if (sommets[triangles[i].s[j]].t == -1)
                sommets[triangles[i].s[j]].t = i;

        // liaison du sommet i_s vers i_s_l
        for (int i_s = 0; i_s < 3; ++i_s)
        {
            int i_s_l = (i_s + 1) % 3;
            ite = finds(connected_faces, {this->triangles[i].s[i_s_l], this->triangles[i].s[i_s]});

            if (ite != connected_faces.end())
            {
                this->triangles[i].adj3[(i_s_l + 1) % 3] = ite->second.first;
                this->triangles[ite->second.first].adj3[ite->second.second] = i;
            }
            else
                connected_faces.insert({{this->triangles[i].s[i_s], this->triangles[i].s[i_s_l]}, {i, (i_s_l + 1) % 3}});
        }
    }
}

void Mesh::load_from_file(std::string f)
{

    std::ifstream myfile;
    myfile.open(f);

    int vertices;
    int faces;
    double v1, v2, v3;
    int s1, s2, s3, bidon;

    if (!myfile.is_open())
    {
        std::cout << "file marche pas" << std::endl;
    }

    myfile >> vertices >> faces >> bidon;

    for (int i = 0; i < vertices; ++i)
    {

        myfile >> v1 >> v2 >> v3;
        this->sommets.emplace_back(v1, v2, v3);
    }

    // this->make_adjacence();

    std::map<std::pair<int, int>, std::pair<int, int>> connected_faces;
    std::map<std::pair<int, int>, std::pair<int, int>>::iterator ite;

    for (int i = 0; i < faces; ++i)
    {
        myfile >> bidon >> s1 >> s2 >> s3;
        this->triangles.emplace_back(s1, s2, s3);

        for (int j = 0; j < 3; ++j)
        {
            if (sommets[triangles[i].s[j]].t == -1)
                sommets[triangles[i].s[j]].t = i;
        }

        // liaison du sommet i_s vers i_s_l
        for (int i_s = 0; i_s < 3; ++i_s)
        {
            int i_s_l = (i_s + 1) % 3;
            ite = finds(connected_faces, {this->triangles[i].s[i_s_l], this->triangles[i].s[i_s]});

            if (ite != connected_faces.end())
            {
                this->triangles[i].adj3[(i_s_l + 1) % 3] = ite->second.first;
                this->triangles[ite->second.first].adj3[ite->second.second] = i;
            }
            else
            {
                connected_faces.insert({{this->triangles[i].s[i_s], this->triangles[i].s[i_s_l]}, {i, (i_s_l + 1) % 3}});
            }
        }
    }

    myfile.close();
}

void Mesh::compute_laplaciens()
{

    Mesh::Iterator_on_vertices ite_vertices = this->vertices_begin();
    Mesh::Iterator_on_vertices ite_vertices_end = this->vertices_end();
    double laplacien_min = 1000, laplacien_max = -1000;

    for (; ite_vertices != ite_vertices_end; ++ite_vertices)
    {
        this->laplaciens.push_back(Algorithm::laplacien(*this, *ite_vertices));
        Point b = laplaciens.back();
        double n = norm(b);

        // b = Algorithm::curvature_from_laplacien(b);
        laplacien_min = std::min(laplacien_min, n);
        laplacien_max = std::max(laplacien_max, n);
    }

    max_curvature = laplacien_max;
    min_curvature = laplacien_min;
}

std::vector<Point> Mesh::load_points_cloud(std::string f)
{
    std::ifstream myfile;
    myfile.open(f);

    int vertices, bidon;
    double v1, v2;

    if (!myfile.is_open())
    {
        std::cout << "file marche pas" << std::endl;
    }

    myfile >> vertices >> bidon >> bidon;

    std::vector<Point> pts;
    pts.reserve(vertices);
    for (int i = 0; i < vertices; ++i)
    {
        myfile >> v1 >> v2;
        pts.emplace_back(v1, v2, 0.);
    }
    return pts;
}