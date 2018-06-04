#include <iostream>

#include "amalgamation.h"

using namespace std;

int main()
{
    LinkageOperation *simple = new SimpleLinkage();
    LinkageCluster l(string("test_dataset"));
    l.print_matrix();
    cout << endl << endl;
    //l.make_clustering(5,simple);
    l.disociative_clustering(2);
    return 0;
}
