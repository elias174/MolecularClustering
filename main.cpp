#include <iostream>

#include "amalgamation.h"

using namespace std;

int main()
{
    LinkageOperation *simple = new SimpleLinkage();
    LinkageOperation *average = new AverageLinkage();
    LinkageOperation *complete = new CompleteLinkage();

    LinkageCluster l(string("diauxic.txt"));
    //l.print_matrix();
    cout << endl << endl;

    cout << "Dissociative linkage with 150 clusters" << endl;
    auto started = std::chrono::high_resolution_clock::now();
    l.disociative_clustering(150);
    auto done = std::chrono::high_resolution_clock::now();
    cout << endl;
    std::cout << std::chrono::duration_cast<std::chrono::microseconds>(done-started).count() << " microseconds " << endl;
    cout << endl << endl;


    //l.disociative_clustering(2);
    return 0;
}
