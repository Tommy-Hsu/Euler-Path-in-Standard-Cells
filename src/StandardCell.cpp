#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <list>
#include <unordered_map>
#include <regex>
#include <vector>
#include <algorithm>
#include <cctype>
#include "StandardCell.h"

std::string toLower(const std::string& str) {
    std::string lowerStr = str;
    std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    return lowerStr;
}

Graph::Graph(/* args */)
{
}

Graph::~Graph()
{
}

StandardCell::StandardCell(/* args */)
{
}

StandardCell::~StandardCell()
{
}

void Graph::FINFETsToGraph(const std::vector<SPICE_FINFET>& FINFETs){
    /* count outgoing & incoming */
    for(size_t i = 0; i < FINFETs.size(); i++){
        std::string lowerDrain = toLower(FINFETs[i].drain);
        std::string lowerSource = toLower(FINFETs[i].source);
        if(FINFETs[i].type == "nmos"){
            /*  nmos, d->s */
            Nodes[lowerSource][0] += 1;
            Nodes[lowerDrain][1] += 1;
        }
        else if(FINFETs[i].type == "pmos"){
            /* pmos, s->d */
            Nodes[lowerSource][1] += 1;
            Nodes[lowerDrain][0] += 1;
        }
    }
}

float StandardCell::extractParameterValue(const std::string& parameter) {
    // 正則表達式匹配 "key=value"，其中 value 可以是浮點數或整數
    std::regex rgx("[a-zA-Z]+=([\\d\\.]+)n?");
    std::smatch match;

    if (std::regex_search(parameter, match, rgx)) {
        // 如果找到匹配，轉換 match[1]（即第一個捕獲組，也就是數值）為浮點數
        return std::stof(match[1].str());
    } else {
        std::cerr << "No numeric value found in the parameter: " << parameter << std::endl;
        return 0.0f; // 或者適當的錯誤處理
    }
}

void StandardCell::parseSPICENetlist(std::ifstream& input){

    std::string line;
    std::string FIN_width, FIN_length, FIN_numFin;

    /* pass first & last line */
    std::getline(input, line);
    while (std::getline(input, line)) {
        if (line.find(".ENDS") != std::string::npos) {
            break;
        }
        std::stringstream ss(line);
        SPICE_FINFET FINFET;
        ss >> FINFET.name >> FINFET.drain >> FINFET.gate >> FINFET.source >> FINFET.substrate >> FINFET.type;
        ss >> FIN_width >> FIN_length >> FIN_numFin;
        FINFET.width = extractParameterValue(FIN_width);
        FINFET.length = extractParameterValue(FIN_length);
        FINFET.numFin = static_cast<size_t>(extractParameterValue(FIN_numFin));   
        FINFETs.push_back(FINFET);
    }
    
    input.close();
    std::cout << " -------- Parse Done --------\n";

}