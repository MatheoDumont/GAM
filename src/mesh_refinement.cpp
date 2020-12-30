#include "mesh.h"
#include "point.h"
#include <cmath>
#include <array>
#include <unordered_set>

typedef std::pair<int, int> Segment;

// https://stackoverflow.com/questions/20590656/error-for-hash-function-of-pair-of-ints

// Graphe planaire de ligne droite
struct PSLG
{
    std::vector<Point> points;
    // un segment <int, int> utilise les indices dans points
    std::vector<Segment> segments;
};

PSLG input()
{
    PSLG g;
    g.points.push_back(Point(0., 0., 0.));
    g.points.push_back(Point(-2., -3.5, 0.));
    g.points.push_back(Point(0., -2, 0.));
    g.points.push_back(Point(1., -3., 0.));
    g.points.push_back(Point(2., -2., 0.));
    g.points.push_back(Point(3., -3., 0.));
    g.points.push_back(Point(4., -2, 0.));
    g.points.push_back(Point(6., -3.5, 0.));
    g.points.push_back(Point(5., 0., 0.));

    g.segments.push_back({0, 1});
    g.segments.push_back({1, 2});
    g.segments.push_back({2, 3});
    g.segments.push_back({3, 4});
    g.segments.push_back({4, 5});
    g.segments.push_back({5, 6});
    g.segments.push_back({6, 7});
    g.segments.push_back({7, 8});

    // point inf et points boite englobante
    for (int i = 0; i < g.segments.size(); ++i)
    {
        g.segments[i].first += 5;
        g.segments[i].second += 5;
    }
    // bounding box
    g.segments.push_back({1, 2});
    g.segments.push_back({1, 3});
    g.segments.push_back({2, 4});
    g.segments.push_back({3, 4});

    return g;
}

std::pair<int, float> Mesh::worst_triangles_angle() const
{
    int idx_worst = 0;
    float worst_angle = triangles[0].lowest_angle(sommets);

    for (int i = 1; i < triangles.size(); ++i)
    {
        if (triangles[i].is_inf)
            continue;
        float a = triangles[i].lowest_angle(sommets);
        if (a < worst_angle)
        {
            idx_worst = i;
            worst_angle = a;
        }
    }
    return {idx_worst, worst_angle};
}

void Mesh::in_circum_to_segments(const Point &p,
                                 const std::unordered_set<Segment, pairhash> &segments,
                                 std::unordered_set<Segment, pairhash> &out) const
{
    for (const Segment &s : segments)
    {
        const Point &p1 = sommets[s.first].p;
        const Point &p2 = sommets[s.second].p;
        Point centre = (p1 + p2) / 2;
        float radius = norm(p1 - p2) / 2;

        if (norm(p - centre) - radius < 0.f)
            out.insert(s);
    }
}

// https://fr.qaz.wiki/wiki/Ruppert's_algorithm
void Mesh::ruppert(float angle_min)
{
    PSLG graph_input = input();

    for (auto p : graph_input.points)
        incremental_delaunay(p);

    // tant que un triangle avec un angle inferieur a un seuil voulu existe
    // on split_segment les segment dans lequel apparait le centre du cercle circonscrit au triangle
    std::unordered_set<Segment, pairhash> segments(graph_input.segments.begin(), graph_input.segments.end());

    std::unordered_set<Segment, pairhash> segs_to_split;
    for (int i = 0; i < triangles.size(); ++i)
    {
        if (triangles[i].is_inf)
            continue;
        in_circum_to_segments(triangles[i].get_barycenter(sommets), segments, segs_to_split);
    }

    int compteur = 0;
    bool stop;
    do
    {
        stop = true;

        for (Segment s : segs_to_split)
        {
            std::array<Segment, 2> ss = split_segment(s);
            segments.insert(ss[0]);
            segments.insert(ss[1]);
        }
        segs_to_split.clear();
        break;
        std::pair<int, float> worst = worst_triangles_angle();
        std::cout << worst.first << ", " << worst.second << std::endl;
        if (worst.second < angle_min)
        {
            stop = false;
            const Point &barycentre = triangles[worst.first].get_barycenter(sommets);

            // on met a jour segs_to_split
            in_circum_to_segments(barycentre, segments, segs_to_split);

            if (segs_to_split.size() == 0) // si aucun
                incremental_delaunay(barycentre);
        }
        compteur++;
        if (compteur == 2)
            stop = true;

    } while (stop == false);
}

std::array<Segment, 2> Mesh::split_segment(Segment s)
{
    /*
     * On commence par trouver le triangle avec le segment
     * ensuite subdivie ce segment en creant 2 triangle 
     * avec pour sommets ceux du triangle avant subdivision + en 3 eme sommets le point median du segment subdivise
     * ensuite on fait les adjacenes des 2 triangles / refait pour celui existent
     * puis les adjacence des triangles adjacent a l'ancien triangle
    */
    int idx_triangle_haut = -1;

    for (int i = 0; i < triangles.size(); ++i)
    {
        const Triangle &t = triangles[i];
        int compteur_sommet_dedans_triangle = 0;
        for (int j = 0; j < 3 && compteur_sommet_dedans_triangle < 2; j++)
            if (t.s[j] == s.first || t.s[j] == s.second)
                compteur_sommet_dedans_triangle++;

        if (compteur_sommet_dedans_triangle == 2)
        {
            idx_triangle_haut = i;
            break;
        }
    }
    assert(idx_triangle_haut != -1);

   

    
    Triangle ancien_triangle_haut = triangles[idx_triangle_haut];
    int idx_rel_sommet_haut_gauche = ancien_triangle_haut.which_vertex(s.first);
    int idx_rel_sommet_haut_droite = ancien_triangle_haut.which_vertex(s.second);

    assert(idx_rel_sommet_haut_droite != -1 && idx_rel_sommet_haut_gauche != -1);
    // au cas ou

    // swap pour garder l'ordre
    if (idx_rel_sommet_haut_droite < idx_rel_sommet_haut_gauche)
    {
        int tmp = idx_rel_sommet_haut_gauche;
        idx_rel_sommet_haut_gauche = idx_rel_sommet_haut_droite;
        idx_rel_sommet_haut_droite = tmp;
    }
    //En faite on modifie 4 triangles, vu qu'il ya 2 triangles sur un edges, on doit modifier les 2 et creer 2 autres.
    int idx_triangle_bas = ancien_triangle_haut.adj3[(idx_rel_sommet_haut_droite + 1) % 3];
    Triangle ancien_triangle_bas = triangles[idx_triangle_bas];

    Point median = sommets[ancien_triangle_haut.s[idx_rel_sommet_haut_gauche]].p + sommets[ancien_triangle_haut.s[idx_rel_sommet_haut_droite]].p;
    median = median / 2;
    sommets.emplace_back(median);
    int idx_median = sommets.size() - 1;
    sommets[idx_median].t = idx_triangle_haut;

    // On cree les triangles et change les sommets
    // HAUT
    Triangle new_triangle_haut_gauche = ancien_triangle_haut;
    Triangle new_triangle_haut_droite = ancien_triangle_haut;

    new_triangle_haut_gauche.s[idx_rel_sommet_haut_droite] = idx_median;
    new_triangle_haut_droite.s[idx_rel_sommet_haut_gauche] = idx_median;

    // BAS
    Triangle new_triangle_bas_gauche = ancien_triangle_bas;
    Triangle new_triangle_bas_droite = ancien_triangle_bas;

    int idx_rel_sommet_bas_gauche = ancien_triangle_bas.which_vertex(s.first);
    int idx_rel_sommet_bas_droite = ancien_triangle_bas.which_vertex(s.second);

    new_triangle_bas_gauche.s[idx_rel_sommet_bas_droite] = idx_median;
    new_triangle_bas_droite.s[idx_rel_sommet_bas_gauche] = idx_median;

    triangles.push_back(new_triangle_haut_droite);
    int idx_new_triangle_haut_droite = triangles.size() - 1;
    triangles.push_back(new_triangle_bas_droite);
    int idx_new_triangle_bas_droite = triangles.size() - 1;

    // adjacences triangles haut
    new_triangle_haut_gauche.adj3[idx_rel_sommet_haut_droite] = ancien_triangle_haut.adj3[idx_rel_sommet_haut_droite];
    new_triangle_haut_droite.adj3[idx_rel_sommet_haut_gauche] = ancien_triangle_haut.adj3[idx_rel_sommet_haut_gauche];

    new_triangle_haut_gauche.adj3[idx_rel_sommet_haut_gauche] = idx_new_triangle_haut_droite;
    new_triangle_haut_droite.adj3[idx_rel_sommet_haut_droite] = idx_triangle_haut;

    new_triangle_haut_gauche.adj3[(idx_rel_sommet_haut_gauche + 2) % 3] = idx_triangle_bas;
    new_triangle_haut_droite.adj3[(idx_rel_sommet_haut_droite + 1) % 3] = idx_new_triangle_bas_droite;

    // adjacences triangles bas
    new_triangle_bas_gauche.adj3[idx_rel_sommet_bas_droite] = ancien_triangle_bas.adj3[idx_rel_sommet_bas_droite];
    new_triangle_bas_droite.adj3[idx_rel_sommet_bas_gauche] = ancien_triangle_haut.adj3[idx_rel_sommet_bas_gauche];

    new_triangle_bas_gauche.adj3[idx_rel_sommet_bas_gauche] = idx_new_triangle_bas_droite;
    new_triangle_bas_droite.adj3[idx_rel_sommet_bas_droite] = idx_triangle_bas;

    new_triangle_bas_gauche.adj3[(idx_rel_sommet_bas_gauche + 1) % 3] = idx_triangle_haut;
    new_triangle_bas_droite.adj3[(idx_rel_sommet_bas_droite + 2) % 3] = idx_new_triangle_haut_droite;

    // adjacences des autres triangles alentour
    int idx_triangle_haut_droite = new_triangle_haut_droite.adj3[idx_rel_sommet_haut_gauche];
    Triangle triangle_haut_droite = triangles[idx_triangle_haut_droite];
    triangle_haut_droite.adj3[triangle_haut_droite.which_adjacence(idx_triangle_haut)] = idx_new_triangle_haut_droite;
    triangles[idx_triangle_haut_droite] = triangle_haut_droite;

    int idx_triangle_bas_droite = new_triangle_bas_droite.adj3[idx_rel_sommet_bas_gauche];
    Triangle triangle_bas_droite = triangles[idx_triangle_bas_droite];
    triangle_bas_droite.adj3[triangle_bas_droite.which_adjacence(idx_triangle_bas)] = idx_new_triangle_bas_droite;
    triangles[idx_triangle_bas_droite] = triangle_bas_droite;

    // on met a jour les triangles de gauche (ceux de droites ont deja ete inseres dans triangles)
    triangles[idx_triangle_haut] = new_triangle_haut_gauche;
    triangles[idx_new_triangle_haut_droite] = new_triangle_haut_droite;
    triangles[idx_triangle_bas] = new_triangle_bas_gauche;
    triangles[idx_new_triangle_bas_droite] = new_triangle_bas_droite;
    
    return {Segment(s.first, idx_median), Segment(idx_median, s.second)};
}