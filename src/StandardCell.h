#pragma once
#include <vector>
#include <set>
#include <unordered_map>
#include <string>

void outputResult(std::ofstream& output, float MinHPWL, 
                    std::vector<std::string> MinPmosResult_, std::vector<std::string> MinNmosResult_,
                    std::vector<std::string> MinPmosFINFETs_, std::vector<std::string> MinNmosFINFETs_);

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
    void reset();
    std::string startEulerian();
    std::vector<std::string> findEulerianPath();
};

class StandardCell {
public:
    /* data */
    std::vector<SPICE_FINFET> FINFETs_;
    std::set<std::string> IOs_;
    Graph nmosGraph;
    Graph pmosGraph;

    std::vector<std::string> PmosSequence_;
    std::vector<std::string> PmosGateSequence_;
    std::vector<std::string> NmosSequence_;
    std::vector<std::string> NmosGateSequence_;

    std::vector<std::string> PmosResult_;
    std::vector<std::string> NmosResult_;

    std::vector<std::string> PmosFINFETs_;
    std::vector<std::string> NmosFINFETs_;

    float HPWL_;

    /* cons & de */
    StandardCell(/* args */);
    ~StandardCell();

    /* function */
    float extractParameterValue(const std::string& parameter);
    std::vector<std::string> findGateSequence(std::unordered_map<std::string, std::unordered_map<std::string, std::vector<std::string>>> SDToGate, const std::vector<std::string>& path);
    std::vector<std::string> findMosSequence(std::unordered_map<std::string, std::vector<std::array<std::string, 2>>> GateToSD, std::vector<std::string>& gateSequence, const std::unordered_map<std::string, size_t>& Nodes);
    std::vector<std::string> mergeIntoMos(const std::vector<std::string>& mosSequence, const std::vector<std::string>& gateSequence);
    std::vector<std::string> SequenceToFINFETs(std::vector<SPICE_FINFET> FINFETs, const std::vector<std::string>& sequence, const std::string& type);
    void alignSequences(std::vector<std::string>& vec1, std::vector<std::string>& vec2);

    /* workflow */
    void parseSPICENetlist(std::ifstream& input);
    void FINFETsToGraph();
    void generateStickDiagram();
    void SequenceToPins();
    void calculateHPWL();
};