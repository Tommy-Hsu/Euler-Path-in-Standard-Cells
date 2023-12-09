#include<iostream>
#include<fstream>
#include "src/StandardCell.h"
using namespace std;

int main(int argc, char* argv[]){

    srand(time(NULL));
    ifstream    input(argv[1]);
    ofstream    output(argv[2]);

    if(!input.is_open()){
        cerr << "Error opening file" << endl;
        exit(1);
    }

    StandardCell standardCell;
    standardCell.parseSPICENetlist(input);
    standardCell.FINFETsToGraph();
    standardCell.generateStickDiagram();
    return 0;   
}