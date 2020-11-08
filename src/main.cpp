#include "mainwindow.h"
#include <QApplication>
#include "mesh.h"
#include <iostream>
#include "algorithm.h"
#include "point.h"

int main(int argc, char *argv[])
{
    // A COMMENTER SI TEST
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();

    // // TEST ITERATOR ET CIRCULATOR
    // Mesh titi;
    // Mesh::Iterator_on_vertices its = titi.vertices_begin();

    // for (its = titi.vertices_begin(); its != titi.vertices_end(); ++its)
    // {
    //     Mesh::Circulator_on_faces cfbegin = titi.incident_faces(*its);
    //     int cmpt = 0;
    //     Mesh::Circulator_on_faces cf = cfbegin;

    //     do
    //     {
    //         cmpt++;
    //         ++cf;
    //     } while (cf != cfbegin);

    //     std ::cout << "valence of the vertex " << cfbegin._get_idx_sommet() << " is " << cmpt << std ::endl;
    // }
    

    // TEST LAPLACIEN
    // std::vector<Point> v_l = Algorithm::vertices_laplacien(titi);
    // for (int i = 0; i < v_l.size(); ++i)
    // {
    //     std::cout << v_l[i] << std::endl;
    // }

}
