#pragma once
#include <list>
#include <vector>
#include <unordered_map>
#include <string>

struct SPICE_FINFET
{
    std::string name;
    std::string drain;
    std::string gate;
    std::string source;
    std::string substrate;
    std::string type;
    float width;
    float length;
    size_t numFin;
};

class Graph {
public:
    /* data */
    std::unordered_map<std::string, std::vector<size_t>> Nodes;

    /* cons & de */
    Graph(/* args */);
    ~Graph();

    /* function */
    void FINFETsToGraph(const std::vector<SPICE_FINFET>& FINFETs);
};

class StandardCell {
public:
    /* data */
    std::vector<SPICE_FINFET> FINFETs;
    Graph graph;

    /* cons & de */
    StandardCell(/* args */);
    ~StandardCell();

    /* function */
    float extractParameterValue(const std::string& parameter);

    /* workflow */
    void parseSPICENetlist(std::ifstream& input);
};
