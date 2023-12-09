#pragma once
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

/* undirected graph */
class Graph {
public:
    /* data */
    std::unordered_map<std::string, size_t> Nodes_;
    std::unordered_map<std::string, std::unordered_map<std::string, std::vector<std::string>>> SDToGate_;
    std::unordered_map<std::string, std::vector<std::array<std::string, 2>>> GateToSD_;
    std::unordered_map<std::string, std::unordered_map<std::string, size_t>> adjMatrix;


    /* cons & de */
    Graph(/* args */);
    ~Graph();

    /* function */
    std::string startEulerian(std::unordered_map<std::string, size_t>& Nodes);
    std::vector<std::string> FindEulerianPath();
};

class StandardCell {
public:
    /* data */
    std::vector<SPICE_FINFET> FINFETs;
    Graph nmosGraph;
    Graph pmosGraph;

    std::vector<std::string> gateSequence_;
    std::vector<std::string> pmosSequence_;
    std::vector<std::string> nmosSequence_;

    /* cons & de */
    StandardCell(/* args */);
    ~StandardCell();

    /* function */
    float extractParameterValue(const std::string& parameter);
    std::vector<std::string> FindGateSequence(std::unordered_map<std::string, std::unordered_map<std::string, std::vector<std::string>>> SDToGate, const std::vector<std::string>& path);
    std::vector<std::string> FindMosSequence(std::unordered_map<std::string, std::vector<std::array<std::string, 2>>> GateToSD, const std::vector<std::string>& gateSequence, const std::unordered_map<std::string, size_t>& Nodes);
    void mergeIntoMos(std::vector<std::string>& mosSequence, const std::vector<std::string>& gateSequence);

    /* workflow */
    void parseSPICENetlist(std::ifstream& input);
    void FINFETsToGraph();
    void generateStickDiagram();
};