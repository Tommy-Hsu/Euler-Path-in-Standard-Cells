#include<iostream>
#include<fstream>
#include "src/StandardCell.h"
using namespace std;

int main(int argc, char* argv[]){

    srand(time(NULL));
    ifstream    input(argv[1]);
    ofstream    output(argv[2]);
    bool isGateSequenceMatched = false;
    size_t count = 0;

    if(!input.is_open()){
        cerr << "Error opening file" << endl;
        exit(1);
    }

    StandardCell standardCell;
    standardCell.parseSPICENetlist(input);
    do{
        standardCell.FINFETsToGraph();
        isGateSequenceMatched = standardCell.generateStickDiagram();
        std::cout << "count: " << count++ << std::endl;
    }while(!isGateSequenceMatched);
    standardCell.SequenceToPins();
    standardCell.calculateHPWL();
    standardCell.outputResult(output);
    return 0;   
}