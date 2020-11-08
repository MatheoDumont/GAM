// Point R(0.4, 0.4, 0.f);
// sommets.emplace_back(R);

// Vrai orientation
// assert(cross(R - sommets[triangles[0].s[0]].p, sommets[triangles[0].s[2]].p - sommets[triangles[0].s[0]].p)._z > 0.f);
// assert(cross(R - sommets[1].p, sommets[3].p - sommets[1].p)._z > 0.f);
// // Faux
// assert(cross(sommets[triangles[0].s[2]].p - sommets[triangles[0].s[0]].p, R - sommets[triangles[0].s[0]].p)._z < 0.f);
// assert(cross(sommets[3].p - sommets[1].p, R - sommets[1].p)._z < 0.f);

// // Vrai orientation methode
// assert(orientation(sommets[triangles[0].s[0]].p, sommets[triangles[0].s[2]].p, R) == 1);
// // Faux
// assert(orientation(sommets[triangles[0].s[0]].p, R, sommets[triangles[0].s[2]].p) == -1);

// // FINIR TESTS

// // Vrai methode in_triangle
// assert(in_triangle(0, R) == 1);
// // Faux
// assert(in_triangle(1, R) == -1);
// assert(locate_triangle(sommets.size() - 1) == 0);

// Point L = Point(0.5, -1., 0.);
// sommets.emplace_back(L);
// for (int i = 0; i < 10; ++i)
// {
//     int t = locate_triangle(sommets.size() - 1);
//     std::cout << "L in " << t << std::endl;

// }
// for (int i = 0; i < triangles.size(); ++i)
// {
//     std::cout << "L in " << i << std::endl;

//     assert(in_triangle(i, L) == -1);
// }

// in_cirscumscribed_triangle
// assert(in_circumscribed_cercle(5, 2, 3, sommets[4].p) == 1);
// assert(in_circumscribed_cercle(5, 2, 3, sommets[1].p) == -1);

// sommets.pop_back(); // remove L
// sommets.pop_back(); // remove R
// Point R = Point(0.4, 0.4, 0.f);
// Point L = Point(0.5, -1., 0.);
// assert(in_triangle(1, R) == -1);
// add_delaunay_point(R);
// assert(in_triangle(1, L) == -1);

// add_delaunay_point(L);

// std::cout << "=====================================" << std::endl;
// for (int i = 0; i < sommets.size(); ++i)
// {
//     std::cout << "[" << i << "]" << std::endl;
//     std::cout << sommets[i] << std::endl;
// }
// for (int i = 0; i < triangles.size(); ++i)
// {
//     std::cout << "[" << i << "]" << std::endl;
//     std::cout << triangles[i] << std::endl;
// }
// std::cout << "=====================================" << std::endl;

// Point D = Point(-1., -1., 0.);
// add_delaunay_point(D);
