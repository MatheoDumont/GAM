#include "mesh.h"
#include "point.h"

static int indice_premier_sommets_voronoi = 0;
void Mesh::crust()
{
    /*
     * Alors pour Crust, je prend le maillage deja fait dans ma boite englobante
     * j'ajoute commes points, les barycentre de mes triangles deja presents
     * avec add_delaunay_point, 
     * puis je supprime ces points et les edges sur lesquels ils sont
     */
    std::vector<Point> pts = load_points_cloud("data/bunny2d.xy");
    for (auto p : pts)
        incremental_delaunay(p);

    indice_premier_sommets_voronoi = sommets.size();
    int old_size = triangles.size();
    for (int i = 6; i < old_size; ++i)
    {
        if (triangles[i].is_inf)
            continue;

        Point bary = triangles[i].get_barycenter(sommets);

        incremental_delaunay(bary);
    }
}

void Mesh::display_contour_crust()
{
    glColor3f(1., 0., 0.);
    for (int i = 6; i < triangles.size(); ++i)
    {
        const Triangle &t = triangles[i];
        if (t.is_inf) continue;
        if (t.s[0] < indice_premier_sommets_voronoi && t.s[1] < indice_premier_sommets_voronoi)
        {
            // glColor3f(1., 0., 0.);

            const Point &p1 = sommets[t.s[0]].p;
            const Point &p2 = sommets[t.s[1]].p;

            glBegin(GL_LINE_STRIP);
            glVertex3f(p1._x, p1._y, p1._z);
            glVertex3f(p2._x, p2._y, p2._z);
            glEnd();
        }
        if (t.s[1] < indice_premier_sommets_voronoi && t.s[2] < indice_premier_sommets_voronoi)
        {
            // glColor3f(1., 0., 0.);

            const Point &p1 = sommets[t.s[1]].p;
            const Point &p2 = sommets[t.s[2]].p;

            glBegin(GL_LINE_STRIP);
            glVertex3f(p1._x, p1._y, p1._z);
            glVertex3f(p2._x, p2._y, p2._z);
            glEnd();
        }
        if (t.s[2] < indice_premier_sommets_voronoi && t.s[0] < indice_premier_sommets_voronoi)
        {
            // glColor3f(1., 0., 0.);

            const Point &p1 = sommets[t.s[2]].p;
            const Point &p2 = sommets[t.s[0]].p;

            glBegin(GL_LINE_STRIP);
            glVertex3f(p1._x, p1._y, p1._z);
            glVertex3f(p2._x, p2._y, p2._z);
            glEnd();
        }
    }
}