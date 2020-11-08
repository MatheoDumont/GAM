#include "algorithm.h"

float Algorithm::aire_triangle(const Point &a, const Point &b, const Point &c)
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

float Algorithm::aire_triangle(const std::vector<Sommet> &v_s, const Triangle &t)
{
    return Algorithm::aire_triangle(v_s[t.s[0]].p, v_s[t.s[1]].p, v_s[t.s[2]].p);
}

Point Algorithm::laplacien(Mesh &m, Sommet &s)
{
    int idx_sommet = m.triangles[s.t].s[m.triangles[s.t].which_vertex(m.sommets, s)];

    // Sommet &s = m.sommets[idx_sommet];
    Point &i = s.p;

    float aire = 0;
    Point u_laplacien;

    Mesh::Circulator_on_faces circ_faces = m.incident_faces(s);
    Mesh::Circulator_on_faces circ_faces_begin = circ_faces;

    std::vector<Triangle> faces;

    do
    {
        aire += Algorithm::aire_triangle(m.sommets, *circ_faces);
        faces.push_back(*circ_faces);
        ++circ_faces;

    } while (circ_faces != circ_faces_begin);
    aire = aire * (1 / 3.0f);

    for (int v = 1; v < faces.size(); ++v)
    {
        Triangle &t = faces[v - 1];
        int i_in_face = t.which_vertex(idx_sommet);
        Point &k = m.sommets[t.s[(i_in_face + 1) % 3]].p;
        Point &j = m.sommets[t.s[(i_in_face + 2) % 3]].p;

        t = faces[v];
        i_in_face = t.which_vertex(idx_sommet);
        Point &kk = m.sommets[t.s[(i_in_face + 2) % 3]].p;

        // vecteurs direction
        Point ji = j - i;
        float length_ji = norm(ji);
        Point ki = i - k;
        Point kki = i - kk;

        // https: //stackoverflow.com/questions/3738384/stable-cotangent
        // sinon arc cos = acos
        float cot_alpha = norm(ki) / length_ji;
        float cot_beta = norm(kki) / length_ji;
        // std::cout << "alpha " << cot_alpha << std::endl;
        // std::cout << "beta " << cot_beta << std::endl;

        u_laplacien = u_laplacien + (cot_alpha + cot_beta) * (ji);
        // std::cout << "laplacien " << u_laplacien << std::endl;
    }

    // return 0.5f * std::max(0.001f, aire) * u_laplacien;
    return (float)(1/(2 * aire)) * u_laplacien;
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
