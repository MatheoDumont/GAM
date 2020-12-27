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

    indice_premier_sommets_voronoi = sommets.size();
    int old_size = triangles.size();
    for (int i = 6; i < old_size; ++i)
    {
        if (triangles[i].is_inf)
            continue;
        Point bary = triangles[i].get_barycenter(sommets);
        // bary._z = 0.f;
        // std::cout << bary << std::endl;
        // if (std::isfinite(bary._x) && std::isfinite(bary._y) && bary._x < 10000 && bary._y < 10000)
        add_delaunay_point(bary);
    }
}

void Mesh::display_contour_crust()
{
    glColor3f(1., 0., 0.);
    for (int i = 6; i < triangles.size(); ++i)
    {
        const Triangle &t = triangles[i];
        for (int j = 1; j < 3; ++j)
            if (t.s[j] < indice_premier_sommets_voronoi && t.s[j - 1] < indice_premier_sommets_voronoi)
            {
                const Point &p1 = sommets[t.s[j]].p;
                const Point &p2 = sommets[t.s[j - 1]].p;

                glBegin(GL_LINE_STRIP);
                glVertex3f(p1._x, p1._y, p1._z);
                glVertex3f(p2._x, p2._y, p2._z);
                glEnd();
            }
    }
}