#include<iostream>
#include<fstream>
#include <limits>
#include <time.h>
#include <cstdlib>
#include "src/StandardCell.h"
using namespace std;

int main(int argc, char* argv[]){

    srand(time(NULL));
    ifstream    input(argv[1]);
    ofstream    output(argv[2]);

    size_t count = 0;
    float MinHPWL = std::numeric_limits<float>::max();
    std::vector<std::string> MinPmosResult_;
    std::vector<std::string> MinNmosResult_;
    std::vector<std::string> MinPmosFINFETs_;
    std::vector<std::string> MinNmosFINFETs_;


    if(!input.is_open()){
        cerr << "Error opening file" << endl;
        exit(1);
    }

    StandardCell standardCell;
    standardCell.parseSPICENetlist(input);
    do{
        standardCell.FINFETsToGraph();
        standardCell.generateStickDiagram();
        standardCell.SequenceToPins();
        standardCell.calculateHPWL();
        if( standardCell.HPWL_ < MinHPWL ){
            MinHPWL = standardCell.HPWL_;
            MinPmosResult_ = standardCell.PmosResult_;
            MinNmosResult_ = standardCell.NmosResult_;
            MinPmosFINFETs_ = standardCell.PmosFINFETs_;
            MinNmosFINFETs_ = standardCell.NmosFINFETs_;
            std::cout << "MinHPWL: " << MinHPWL << std::endl;
        }
        count++;
    }while( count < standardCell.FINFETs_.size() * 10000);
    outputResult(output, MinHPWL, MinPmosResult_, MinNmosResult_, MinPmosFINFETs_, MinNmosFINFETs_);
    return 0;   
}