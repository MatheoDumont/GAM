#include "mesh.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "algorithm.h"
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
}

Mesh::Mesh() : laplaciens(), sommets(), triangles()
{
    // Point infini
    this->make_inf_point();

    sommets.emplace_back(0.f, 0.f, 0.f);
    sommets.emplace_back(1.f, 0.f, 0.f);
    sommets.emplace_back(0.f, 1.f, 0.f);
    sommets.emplace_back(1.f, 1.f, 0.f);

    triangles.emplace_back(1, 2, 3);
    triangles.emplace_back(2, 4, 3);
    this->make_adjacence();
    this->make_infinite_triangles();

    // this->tetrahedron();
    // load_from_file("data/queen.off");
    // load_from_file("data/cube.off");
    // this->compute_laplaciens();
    // this->edge_flip(0, 1);

    // sommets.emplace_back(0.25, 0.25, 0.f);
    // this->triangle_split(0, 4);

    // this->edge_flip(0, 1);
    // add_delaunay_point(Point(0.25, 0.25, 0.f));
    load_and_add_delaunay("data/franke4.off");
}

void Mesh::drawMesh()
{
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_real_distribution<> dis(0.f, 1.0f);

    for (unsigned int i = 0; i < this->triangles.size(); ++i)
    {
        Triangle &t = triangles[i];

        glBegin(GL_TRIANGLES);

        // if (triangles[i].is_inf)
        // {
        //     glColor3f(1., 1., 1.);
        // }
        // else
        // {
        //     glColor3f(1., 0., 0.);
        // }

        glColor3f(dis(gen), dis(gen), dis(gen));

        glPointDraw(sommets[t.s[0]].p);

        glPointDraw(sommets[t.s[1]].p);

        glPointDraw(sommets[t.s[2]].p);

        glEnd();
    }
}

void Mesh::drawMeshWireFrame()
{
    for (unsigned int i = 0; i < this->triangles.size(); ++i)
    {

        if (i == triangles.size() - 2)
        {
            glColor3f(0., 1., 0.);
        }
        else if (i == triangles.size() - 1)
        {
            glColor3f(0., 0., 1.);
        }
        else if (triangles[i].is_inf)
        {
            glColor3f(1., 1., 1.);
        }
        else
        {
            glColor3f(1., 0., 0.);
        }

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

    for (; ite_vertices != ite_vertices_end; ++ite_vertices)
    {
        this->laplaciens.push_back(Algorithm::laplacien(*this, *ite_vertices));
    }
}