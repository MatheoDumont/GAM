#include "algorithm.h"

double Algorithm::aire_triangle(const Point &a, const Point &b, const Point &c)
{
    //https://fr.wikipedia.org/wiki/Aire_d%27un_triangle
    Point ab = b - a;
    Point ac = c - a;
    // Point pv = cross(ab, ac);
    // float length = norm(pv);
    // float aire = (1 / 2) * norm(cross(ab, ac));
    // float aire = 0.5f * length;
    // std::cout << "Produit Vectoriel : " << pv << " length : " << length << " resultat de l'aire : " << aire << std::endl;

    return 0.5f * norm(cross(ab, ac));
}

double Algorithm::aire_triangle(const std::vector<Sommet> &v_s, const Triangle &t)
{
    return Algorithm::aire_triangle(v_s[t.s[0]].p, v_s[t.s[1]].p, v_s[t.s[2]].p);
}

float cos_geom(const Point &a, const Point &i, const Point &j)
{
    Point ai = normalize(i - a);
    Point aj = normalize(j - a);
    return dot(ai, aj);
}

float sin_geom(const Point &a, const Point &i, const Point &j)
{
    Point ai = i - a;
    Point aj = j - a;
    float length_ai = norm(ai);

    return (length_ai * length_ai) - dot(ai, aj) / length_ai;
}

float cot_geom(const Point &a, const Point &i, const Point &j)
{
    return cos_geom(a, i, j) / sin_geom(a, i, j);
}

double cot_geom_2(const Point &i, const Point &j, const Point &angle_point)
{

    Point a = normalize(i - angle_point);
    Point b = normalize(j - angle_point);

    double cos_t = dot(a, b);
    double sin_t = norm(cross(a, b));

    return cos_t / sin_t;
}

double clamp(double min, double max, double x)
{
    if (x >= max)
        return max;
    else if (x <= min)
        return min;

    return x;
}
// http://rodolphe-vaillant.fr/?e=33
// http://rodolphe-vaillant.fr/?e=69
Point Algorithm::laplacien(Mesh &m, Sommet &sommet)
{
    int idx_sommet = m.triangles[sommet.t].s[m.triangles[sommet.t].which_vertex(m.sommets, sommet)];

    Mesh::Circulator_on_faces circ_faces = m.incident_faces(sommet);
    Mesh::Circulator_on_faces circ_faces_begin = circ_faces;

    double aire = 0.0;
    do
    {
        aire += Algorithm::aire_triangle(m.sommets, *circ_faces);
        ++circ_faces;
    } while (circ_faces != circ_faces_begin);

    aire = aire * (1.0 / 3.0) * 2.0;
    aire = 1.0 / aire;

    Mesh::Circulator_on_faces circ_faces_p = m.incident_faces(sommet);
    Mesh::Circulator_on_faces circ_faces_begin_p = circ_faces_p;

    Mesh::Circulator_on_faces circ_faces_after_p = m.incident_faces(sommet);
    ++circ_faces_after_p;

    Point u_laplacien;
    const Point &i = sommet.p;

    do
    {
        const Triangle &t_precedent = m.triangles[circ_faces_p.idx];
        const Triangle &t_suivant = m.triangles[circ_faces_after_p.idx];

        int i_dans_t_prec = t_precedent.which_vertex(idx_sommet);
        int i_dans_t_suiv = t_suivant.which_vertex(idx_sommet);

        const Point &k = m.sommets[t_precedent.s[(i_dans_t_prec + 1) % 3]].p;
        const Point &j = m.sommets[t_precedent.s[(i_dans_t_prec + 2) % 3]].p;
        const Point &kk = m.sommets[t_suivant.s[(i_dans_t_suiv + 2) % 3]].p;

        // float cot_alpha = cot_geom(k, i, j);
        // float cot_beta = cot_geom(kk, i, j);

        float cot_alpha = cot_geom_2(i, j, k);
        float cot_beta = cot_geom_2(i, j, kk);

        float weight = cot_alpha + cot_beta;

        u_laplacien = u_laplacien + weight * (i - j);

        ++circ_faces_p;
        ++circ_faces_after_p;

    } while (circ_faces_p != circ_faces_begin_p);

    return aire * u_laplacien;
}

std::vector<Point> Algorithm::vertices_laplacien(Mesh &m)
{
    std::vector<Point> vec_u;

    Mesh::Iterator_on_vertices ite_vertices = m.vertices_begin();
    Mesh::Iterator_on_vertices ite_vertices_end = m.vertices_end();

    for (; ite_vertices != ite_vertices_end; ++ite_vertices)
    {
        vec_u.push_back(Algorithm::laplacien(m, *ite_vertices));
    }
    return vec_u;
}

Point Algorithm::normal_from_laplacien(const Point &laplacien)
{
    return laplacien / norm(laplacien);
}

Point Algorithm::curvature_from_laplacien(const Point &laplacien)
{
    /*
    * delta_s = 2Hn
    * delta_s : le laplacien en s
    * H la courbure en ce point
    * s la normal en ce point
    */
    return 0.5f * laplacien * normal_from_laplacien(laplacien);
}
