#include "mesh.h"
#include <cassert>
const int Mesh::IDX_INF_POINT;

void Mesh::make_inf_point()
{
    // le point inf doit etre le premier creer
    assert(sommets.size() == 0);
    // pos (1., 1., 5.) pour etre sur que le produit vectoriel
    // nous donne pas 0 en z
    sommets.emplace_back(1.f, 1.f, 5.f);
}

void Mesh::make_infinite_triangles()
{
    int s = triangles.size();
    for (int i = 0; i < s; ++i)
        for (int j = 0; j < 3; ++j)
            if (triangles[i].adj3[j] == -1)
            {
                triangles.emplace_back(triangles[i].s[(j + 2) % 3],
                                       triangles[i].s[(j + 1) % 3],
                                       Mesh::IDX_INF_POINT,
                                       /* triangle infini*/ true);
            }
    make_adjacence();
}

void Mesh::triangle_split(int idx_triangle, int idx_sommet, bool is_inf)
{
    // premier triangle, celui qu'on change juste
    Triangle trig = this->triangles[idx_triangle];

    // sommet du triangle trig
    int s0, s1, s2;
    s0 = trig.s[0];
    s1 = trig.s[1];
    s2 = trig.s[2];

    // triangle incident au sommet ajoute
    sommets[idx_sommet].t = idx_triangle;

    // Le triangle qu'on modifie
    this->triangles[idx_triangle].s[1] = idx_sommet;

    // Les deux triangles qu'on cree
    triangles[idx_triangle].is_inf = is_inf;
    Triangle t2(s1, idx_sommet, s0, is_inf);
    Triangle t3(s2, idx_sommet, s1, is_inf);

    // on ajoute les triangles
    this->triangles.push_back(t2);
    this->triangles.push_back(t3);

    // Adjacences
    int idx_t2 = this->triangles.size() - 2;
    int idx_t3 = this->triangles.size() - 1;

    this->triangles[idx_t2].update_3adj(idx_triangle, trig.adj3[2], idx_t3);
    this->triangles[idx_t3].update_3adj(idx_t2, trig.adj3[0], idx_triangle);
    this->triangles[idx_triangle].update_3adj(idx_t3, trig.adj3[1], idx_t2);

    // Triangle0
    // et on update pour chaque triangle adjacent, leur adjacence
    // triangle adjacent a la face pointe par le sommet 0 de idx_triangle
    int triangle_adj0 = trig.adj3[0];
    if (triangle_adj0 != -1)
    {
        // parmis tous ses sommets, lequel regarde idx_triangle
        int triangle_adj0_som = triangles[triangle_adj0].which_adjacence(idx_triangle);
        triangles[triangle_adj0].adj3[triangle_adj0_som] = idx_t3;
    }

    // Triangle1
    int triangle_adj1 = trig.adj3[1];
    if (triangle_adj1 != -1)
    {
        int triangle_adj1_som = triangles[triangle_adj1].which_adjacence(idx_triangle);
        triangles[triangle_adj1].adj3[triangle_adj1_som] = idx_triangle;
    }

    // Triangle2
    int triangle_adj2 = trig.adj3[2];
    if (triangle_adj2 != -1)
    {
        int triangle_adj2_som = triangles[triangle_adj2].which_adjacence(idx_triangle);
        triangles[triangle_adj2].adj3[triangle_adj2_som] = idx_t2;
    }
}

bool Mesh::is_flippable(int idx_t1, int idx_t2)
{
    if (triangles[idx_t1].is_inf != triangles[idx_t2].is_inf)
        return false;

    int idx_s_t1_voit_t2 = triangles[idx_t1].which_adjacence(idx_t2);
    int idx_s_t2_voit_t1 = triangles[idx_t2].which_adjacence(idx_t1);

    if (idx_s_t1_voit_t2 == -1 || idx_s_t2_voit_t1 == -1)
        return false;

    const Point &point_t1_voit_t2 = sommets[triangles[idx_t1].s[idx_s_t1_voit_t2]].p;
    const Point &point_t2_voit_t1 = sommets[triangles[idx_t2].s[idx_s_t2_voit_t1]].p;
    const Point &point_pivot_bas = sommets[triangles[idx_t1].s[(idx_s_t1_voit_t2 + 1) % 3]].p;
    const Point &point_pivot_haut = sommets[triangles[idx_t1].s[(idx_s_t1_voit_t2 + 2) % 3]].p;

    if (orientation(point_pivot_haut, point_t2_voit_t1, point_t1_voit_t2) == 1 &&
        orientation(point_pivot_bas, point_t1_voit_t2, point_t2_voit_t1) == 1)
        return true;
    else
        return false;
}

void Mesh::edge_flip(int idx_t1, int idx_t2)
{
    Triangle t1_old = this->triangles[idx_t1];
    Triangle t2_old = this->triangles[idx_t2];

    // sommets qui font face a l'autre triangle avec lequel on flip
    // indices relatifs
    int t1_s_face = triangles[idx_t1].which_adjacence(idx_t2);
    int t2_s_face = triangles[idx_t2].which_adjacence(idx_t1);

    // assert(!(t1_s_face < 0 || t2_s_face < 0));

    if (t1_s_face < 0 || t2_s_face < 0)
    {
        std::cout << "EDGE FLIP : ERROR, -1 pas cense etre dans t1_s_face ou t2_s_face" << std::endl;
        std::cout << "On essaie de flipper T1 :" << idx_t1 << std::endl
                  << triangles[idx_t1] << std::endl
                  << " et T2 " << idx_t2 << std::endl
                  << triangles[idx_t2] << std::endl;
        return;
    }

    // on fait le split
    this->triangles[idx_t1].s[0] = t1_old.s[t1_s_face];
    this->triangles[idx_t1].s[1] = t1_old.s[(t1_s_face + 1) % 3];
    this->triangles[idx_t1].s[2] = t2_old.s[t2_s_face];

    this->triangles[idx_t2].s[0] = t2_old.s[t2_s_face];
    this->triangles[idx_t2].s[1] = t2_old.s[(t2_s_face + 1) % 3];
    this->triangles[idx_t2].s[2] = t1_old.s[t1_s_face];

    // on repare les adjacences
    this->triangles[idx_t1].adj3[0] = t2_old.adj3[(t2_s_face + 1) % 3];
    this->triangles[idx_t1].adj3[1] = idx_t2;
    this->triangles[idx_t1].adj3[2] = t1_old.adj3[(t1_s_face + 2) % 3];

    this->triangles[idx_t2].adj3[0] = t1_old.adj3[(t1_s_face + 1) % 3];
    this->triangles[idx_t2].adj3[1] = idx_t1;
    this->triangles[idx_t2].adj3[2] = t2_old.adj3[(t2_s_face + 2) % 3];

    // on repare les adjacences des autres triangles
    int idx_adj_t1 = t2_old.adj3[(t2_s_face + 1) % 3];
    int idx_adj_t2 = t2_old.adj3[(t2_s_face + 2) % 3];
    int idx_adj_t3 = t1_old.adj3[(t1_s_face + 1) % 3];
    int idx_adj_t4 = t1_old.adj3[(t1_s_face + 2) % 3];

    triangles[idx_adj_t1].adj3[triangles[idx_adj_t1].which_adjacence(idx_t2)] = idx_t1;
    triangles[idx_adj_t2].adj3[triangles[idx_adj_t2].which_adjacence(idx_t2)] = idx_t2;
    triangles[idx_adj_t3].adj3[triangles[idx_adj_t3].which_adjacence(idx_t1)] = idx_t2;
    triangles[idx_adj_t4].adj3[triangles[idx_adj_t4].which_adjacence(idx_t1)] = idx_t1;
}

int Mesh::orientation(int p, int q, int r) const
{
    return orientation(sommets[p].p, sommets[q].p, sommets[r].p);
}

int Mesh::orientation(const Point &p, const Point &q, const Point &r) const
{
    // Point p1(p._x, p._y, 0.f);
    // Point p2(q._x, q._y, 0.f);
    // Point p3(r._x, r._y, 0.f);
    // float dir = cross(p3 - p1, p2 - p1)._z;

    // Point p11 = p3 - p1;
    // Point p21 = p2 - p1;
    Point p_prim = Point(p._x, p._y, 0.);
    Point pr = Point(r._x, r._y, 0.f) - p_prim;
    Point pq = Point(q._x, q._y, 0.f) - p_prim;
    float dir = (pr._x * pq._y) - (pr._y * pq._x);

    if (dir == 0.f)
        return 0;
    else if (dir > 0)
        return 1;

    return -1;
}

int Mesh::in_triangle(int t, const Point &p) const
{
    int orientation_plan1 = orientation(sommets[triangles[t].s[1]].p, sommets[triangles[t].s[0]].p, p);
    int orientation_plan2 = orientation(sommets[triangles[t].s[2]].p, sommets[triangles[t].s[1]].p, p);
    int orientation_plan3 = orientation(sommets[triangles[t].s[0]].p, sommets[triangles[t].s[2]].p, p);

    if (orientation_plan1 == 0 || orientation_plan2 == 0 || orientation_plan3 == 0)
        return 0;

    if (orientation_plan1 == -1 || orientation_plan2 == -1 || orientation_plan3 == -1)
        return -1;

    return 1;
}

int Mesh::in_circumscribed_cercle(int idx_triangle, int idx_point) const
{
    int i_p = triangles[idx_triangle].s[0];
    int i_q = triangles[idx_triangle].s[1];
    int i_r = triangles[idx_triangle].s[2];
    Point s = sommets[idx_point].p;

    // on calcul les points par rapport a p considere comme le centre
    // du paraboloid
    // q et r centrer par rapport a p
    Point q = sommets[i_q].p;
    Point r = sommets[i_r].p;
    Point p = sommets[i_p].p;

    // on les remontes dans le saladier
    p._z = 0.;
    q._z = ((q._x - p._x) * (q._x - p._x)) + ((q._y - p._y) * (q._y - p._y));
    r._z = ((r._x - p._x) * (r._x - p._x)) + ((r._y - p._y) * (r._y - p._y));
    s._z = ((s._x - p._x) * (s._x - p._x)) + ((s._y - p._y) * (s._y - p._y));

    Point normal = cross(q - p, r - p);
    float dir = dot(normal, s - p);

    if (dir == 0.f)
        return 0;
    else if (dir < 0)
        return 1;

    return -1;
}

int Mesh::locate_triangle_iterative(int idx_p) const
{
    const Point &pt = sommets[idx_p].p;

    for (int i = 0; i < triangles.size(); ++i)
    {
        if (in_triangle(i, pt) >= 0)
            return i;
    }
    return -1;
}

int Mesh::locate_triangle_fourmis(int idx_p) const
{
    // unorderet set pour savoir si on revient sur nos pas
    std::unordered_set<int> set;

    const Point &pt = sommets[idx_p].p;
    std::mt19937 generator;

    int sommet_pivot, s2_rel, s3_rel;
    // Point point_pivot, p2, p3;
    // int current_t = std::uniform_int_distribution<int>(0, triangles.size())(generator);
    int current_t = 0;

    if (in_triangle(current_t, pt) >= 0)
        return current_t;

    int previous_t = -1;
    const Triangle &t = triangles[current_t];
    for (int i = 0; i < 3; i++)
    {
        if (orientation(t.s[i], t.s[(i + 1) % 3], idx_p) > 0)
        {
            previous_t = current_t;
            current_t = t.adj3[(i + 2) % 3];
            break;
        }
    }

    if (previous_t == -1)
    {
        std::cout << "locate_triangle : erreur, aucune orientation valide trouvee" << std::endl;
        exit(1);
    }

    set.insert(previous_t);
    // set.insert(current_t);

    while (1)
    {
        if (set.find(current_t) != set.end())
            return -1; // on est deja venue sur ce triangle avant

        if (in_triangle(current_t, pt) >= 0)
        {
            if (triangles[current_t].is_inf)
                return -1;
            else
                return current_t;
        }

        const Triangle &t = triangles[current_t];
        sommet_pivot = -1;

        for (int i = 0; i < 3; ++i)
        {
            if (t.adj3[i] == previous_t)
            {
                sommet_pivot = i;
                break;
            }
        }

        const Point &point_pivot = sommets[t.s[sommet_pivot]].p;
        s2_rel = (sommet_pivot + 1) % 3;
        s3_rel = (sommet_pivot + 2) % 3;

        const Point &p2 = sommets[t.s[s2_rel]].p;
        const Point &p3 = sommets[t.s[s3_rel]].p;

        if (orientation(p3, point_pivot, pt) > 0)
        {
            previous_t = current_t;
            current_t = t.adj3[s2_rel];
        }
        else if (orientation(point_pivot, p2, pt) > 0)
        {
            previous_t = current_t;
            current_t = t.adj3[s3_rel];
        }
        else
            return -1;

        set.insert(previous_t);
    }

    return -1;
}

void Mesh::add_delaunay_point(Point p)
{
    sommets.emplace_back(p);
    int idx_s = sommets.size() - 1;
    int idx_t = -1;
    idx_t = locate_triangle_fourmis(idx_s);
    // assert(idx_t != -1 /* ON ne trouve pas le triangle contenant le point */);

    if (idx_t == -1 || triangles[idx_t].is_inf) // pas dans l'enveloppe convex
    {
        std::cout << "OUTSIDE" << std::endl;
        Mesh::Circulator_on_faces cfbegin = this->incident_faces(sommets[IDX_INF_POINT]);
        Mesh::Circulator_on_faces cf = cfbegin;
        int idx_triangle_splitted = -1;

        do // on cherche le triangle dans lequel on split
        {
            int idx_s_inf_on_face = cf->which_vertex(IDX_INF_POINT);

            if (orientation(cf->s[(idx_s_inf_on_face + 2) % 3],
                            cf->s[(idx_s_inf_on_face + 1) % 3],
                            idx_s) == 1)
            // LE TRIANGLE REMONTE DANS LENVELOPPE CONVEX COMME ETANT NON INF
            {

                triangle_split(cf.idx, idx_s, true /*tous infini*/);

                // on cherche le triangle qui n'est plus infini
                // celui-ci ne possede plus pour sommet le sommet infini
                if (triangles[cf.idx].which_vertex(Mesh::IDX_INF_POINT) == -1)
                {
                    triangles[cf.idx].is_inf = false;
                    idx_triangle_splitted = cf.idx;
                }
                else if (triangles[triangles.size() - 2].which_vertex(Mesh::IDX_INF_POINT) == -1)
                {
                    triangles[triangles.size() - 2].is_inf = false;
                    idx_triangle_splitted = triangles.size() - 2;
                }
                else
                {
                    triangles[triangles.size() - 1].is_inf = false;
                    idx_triangle_splitted = triangles.size() - 1;
                }
                std::cout << triangles[cf.idx] << std ::endl
                          << triangles[triangles.size() - 2] << std::endl
                          << triangles[triangles.size() - 1] << std::endl;

                std::cout << "===========================================" << std::endl;

                break;
            }
            ++cf;
        } while (cf != cfbegin);

        int idx_point_ajoute_dans_triangle_flipped = triangles[idx_triangle_splitted].which_vertex(idx_s);

        // 1 on regarde a gauche du triangle si ya un flip a faire
        {
            int idx_s_cote_1 = (idx_point_ajoute_dans_triangle_flipped + 1) % 3;
            int idx_triangle_adj1 = triangles[idx_triangle_splitted].adj3[idx_s_cote_1];
            int idx_s_dans_triangle_adj1 = triangles[idx_triangle_adj1].which_vertex(idx_s);

            int idx_triangle_adj2 = triangles[idx_triangle_adj1].adj3[idx_s_dans_triangle_adj1];
            int idx_sommet_triangle_adj2_pointe_sur_triangle_adj1 = triangles[idx_triangle_adj2].which_adjacence(idx_triangle_adj1);
            int idx_sommet2_triangle_adj2_pas_infini = -1;

            if (triangles[idx_triangle_adj2].s[(idx_sommet_triangle_adj2_pointe_sur_triangle_adj1 + 1) % 3] != 0)
                idx_sommet2_triangle_adj2_pas_infini = (idx_sommet_triangle_adj2_pointe_sur_triangle_adj1 + 1) % 3;
            else
                idx_sommet2_triangle_adj2_pas_infini = (idx_sommet_triangle_adj2_pointe_sur_triangle_adj1 + 2) % 3;

            if (orientation(
                    sommets[triangles[idx_triangle_adj2].s[idx_sommet2_triangle_adj2_pas_infini]].p,
                    sommets[idx_s].p,
                    sommets[triangles[idx_triangle_adj2].s[idx_sommet_triangle_adj2_pointe_sur_triangle_adj1]].p) == 1)
            //&& is_flippable(idx_triangle_adj1, idx_triangle_adj2)
            {

                edge_flip(idx_triangle_adj1, idx_triangle_adj2);

                if (triangles[idx_triangle_adj1].which_vertex(idx_sommet2_triangle_adj2_pas_infini) != -1)
                    triangles[idx_triangle_adj2].is_inf = false;
                else
                    triangles[idx_triangle_adj1].is_inf = false;
            }
        }

        // 2 on regarde a droite du triangle
        {
            int idx_s_cote_2 = (idx_point_ajoute_dans_triangle_flipped + 2) % 3;
            int idx_triangle_adj1 = triangles[idx_triangle_splitted].adj3[idx_s_cote_2];
            int idx_s_dans_triangle_adj1 = triangles[idx_triangle_adj1].which_vertex(idx_s);

            int idx_triangle_adj2 = triangles[idx_triangle_adj1].adj3[idx_s_dans_triangle_adj1];
            int idx_sommet_triangle_adj2_pointe_sur_triangle_adj1 = triangles[idx_triangle_adj2].which_adjacence(idx_triangle_adj1);
            int idx_sommet2_triangle_adj2_pas_infini = -1;

            if (triangles[idx_triangle_adj2].s[(idx_sommet_triangle_adj2_pointe_sur_triangle_adj1 + 1) % 3] != 0)
                idx_sommet2_triangle_adj2_pas_infini = (idx_sommet_triangle_adj2_pointe_sur_triangle_adj1 + 1) % 3;
            else
                idx_sommet2_triangle_adj2_pas_infini = (idx_sommet_triangle_adj2_pointe_sur_triangle_adj1 + 2) % 3;

            if (orientation(
                    sommets[triangles[idx_triangle_adj2].s[idx_sommet2_triangle_adj2_pas_infini]].p,
                    sommets[triangles[idx_triangle_adj2].s[idx_sommet_triangle_adj2_pointe_sur_triangle_adj1]].p,
                    sommets[idx_s].p) == 1) //&& is_flippable(idx_triangle_adj1, idx_triangle_adj2)
            {

                edge_flip(idx_triangle_adj1, idx_triangle_adj2);

                if (triangles[idx_triangle_adj1].which_vertex(idx_sommet2_triangle_adj2_pas_infini) != -1)
                    triangles[idx_triangle_adj1].is_inf = false;
                else
                    triangles[idx_triangle_adj2].is_inf = false;
            }
        }
    }

    else // dans l'enveloppe convex
    {
        std::cout << "INSIDE" << std::endl;

        // ON CONSIDERE T1 COMME ETANT LE TRIANGLE AYANT DEJA SUBIT DES
        // POTENTIELS FLIP, DONC T2 EST CELUI A MODIFIER PEUT ETRE

        triangle_split(idx_t, idx_s, false);

        std::queue<std::pair<int, int>> queue;

        // INIT queue avec des pairs de triangles :
        // les triangles issuent du split et leurs triangles adjacent par rapport au point insere.
        int s_in_idxt = triangles[idx_t].which_vertex(idx_s);
        queue.push({idx_t, triangles[idx_t].adj3[s_in_idxt]});

        int t2 = triangles.size() - 2;
        int s_in_t2 = triangles[t2].which_vertex(idx_s);
        queue.push({t2, triangles[t2].adj3[s_in_t2]});

        int t3 = triangles.size() - 1;
        int s_in_t3 = triangles[t3].which_vertex(idx_s);
        queue.push({t3, triangles[t3].adj3[s_in_t3]});

        while (!queue.empty())
        {
            std::pair<int, int> p = queue.front();
            queue.pop();
            int idx_t1 = p.first;
            int idx_t2 = p.second;

            // on flip pas les edges des triangles infinis
            if (triangles[idx_t1].is_inf || triangles[idx_t2].is_inf)
                continue;

            // donc en idx_t1 dans les pairs, idx_t1 toujours le triangle avec le sommet ajoute.
            int idx_s_de_t2_adj_t1 = triangles[idx_t2].which_adjacence(idx_t1);

            if (in_circumscribed_cercle(idx_t1, triangles[idx_t2].s[idx_s_de_t2_adj_t1]) == 1 &&
                is_flippable(idx_t1, idx_t2))
            {

                edge_flip(idx_t1, idx_t2);

                int s_in_first = triangles[idx_t1].which_vertex(idx_s);
                if (s_in_first != -1)
                    queue.push({idx_t1, triangles[idx_t1].adj3[s_in_first]});

                int s_in_second = triangles[idx_t2].which_vertex(idx_s);
                if (s_in_second != -1)
                    queue.push({idx_t2, triangles[idx_t2].adj3[s_in_second]});
            }
        }
    }
}

void Mesh::load_and_add_delaunay(std::string file)
{
    std::ifstream myfile(file);

    if (!myfile.is_open())
    {
        std::cout << "file marche pas" << std::endl;
        exit(1);
    }
    std::string entete;
    myfile >> entete;

    int vertices = 0, faces = 0, bidon = 0;
    myfile >> vertices >> faces >> bidon;

    double v1, v2, v3;

    for (int i = 0; i < vertices; ++i)
    {
        myfile >> v1 >> v2 >> v3;
        Point p(v1, v2, 0.);
        this->add_delaunay_point(p);
    }

    myfile.close();
}

void Mesh::incremental_delaunay(Point p)
{
    if (sommets.size() <= 2 + 1)
    {
        sommets.emplace_back(p);
        if (sommets.size() == 3 + 1)
        {
            if (orientation(1, 2, 3) == -1)
                triangles.emplace_back(1, 2, 3);
            else
                triangles.emplace_back(1, 3, 2);

            this->make_infinite_triangles();
        }
    }
    else
    {
        add_delaunay_point(p);
        check_adjacence();
    }
}

void Mesh::boite_englobante(float k)
{
    sommets.emplace_back(-k, -k, 0.f);
    sommets.emplace_back(k, -k, 0.f);
    sommets.emplace_back(-k, k, 0.f);
    sommets.emplace_back(k, k, 0.f);

    triangles.emplace_back(1, 2, 3);
    triangles.emplace_back(2, 4, 3);
    this->make_adjacence();
    this->make_infinite_triangles();
}

void Mesh::check_adjacence()
{
    for (int j = 0; j < triangles.size(); ++j)
    {
        const Triangle &t = triangles[j];
        for (int i = 0; i < 3; ++i)
        {
            if (triangles[t.adj3[i]].which_adjacence(j) == -1)
            {
                std::cout << "ERROR" << std::endl
                          << "l'un adj mais pas l'autre " << std::endl
                          << triangles[i]
                          << triangles[t.adj3[i]]
                          << std::endl;
                break;
            }
        }
    }
    // check si un triangle est inf sans avoir le point 0 dans ses sommets
    for (int i = 0; i < triangles.size(); ++i)
    {
        const Triangle &t = triangles[i];
        bool point_inf_dans_sommets = false;
        for (int j = 0; j < triangles.size(); ++j)
            if (t.s[j] == IDX_INF_POINT)
            {
                point_inf_dans_sommets = true;
                break;
            }

        if ((point_inf_dans_sommets && !t.is_inf) ||
            (!point_inf_dans_sommets && t.is_inf))
        {
            std::cout << "!! Incoherence !!" << std::endl;
            std::cout << t << std::endl;
            break;
        }
    }
}