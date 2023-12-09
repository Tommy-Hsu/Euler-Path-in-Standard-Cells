#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <regex>
#include <vector>
#include <algorithm>
#include <cctype>
#include <stack>
#include <array>
#include "StandardCell.h"

Graph::Graph(/* args */)
{
}

Graph::~Graph()
{
}

/* function */
std::string Graph::startEulerian(std::unordered_map<std::string, size_t>& Nodes)
{
    // Check if all non-zero degree vertices are connected

    int odd = 0;
    std::string oddNodeKey;
    for(auto& node : Nodes){
        if(node.second % 2 == 1){
            odd++;
            oddNodeKey = node.first;
        }
    }
    if (odd > 2) {
        return ""; // 无欧拉路径或欧拉回路
    } else if (odd == 2) {
        return oddNodeKey; // 返回具有奇数度的顶点的键
    } else {
        // 返回随机键
        size_t index = std::rand() % Nodes.size();
        auto it = Nodes.begin();
        std::advance(it, index);
        return it->first;
    }
}

std::vector<std::string> Graph::FindEulerianPath()
{
    /* 
        assignment guarantee exit euler path 
        by adding dummy edge (gate) to the graph
    */
    std::unordered_map<std::string, std::unordered_map<std::string, size_t>> temp_adjMatrix = this->adjMatrix;
    std::stack<std::string> stack;
    std::vector<std::string> path;
    std::string startpoint = startEulerian(Nodes_);
    if(startpoint == ""){
        std::cout << " -------- No Eulerian Path --------\n";
        return path;
    }

    std::string cur = startpoint;

    while(!stack.empty() || temp_adjMatrix[cur].size() > 0){
        if(temp_adjMatrix[cur].size() == 0){
            path.push_back(cur);
            cur = stack.top();
            stack.pop();
        }
        else{
            stack.push(cur);
            std::string next = temp_adjMatrix[cur].begin()->first;
            temp_adjMatrix[cur][next] -= 1;
            temp_adjMatrix[next][cur] -= 1;
            if(temp_adjMatrix[cur][next] == 0){
                temp_adjMatrix[cur].erase(next);
                temp_adjMatrix[next].erase(cur);
            }
            cur = next;
        }
    }
    path.push_back(cur);
    std::cout << " -------- Eulerian Path --------\n";
    return path;
}

StandardCell::StandardCell(/* args */)
{
}

StandardCell::~StandardCell()
{
}

/* function */
void StandardCell::mergeIntoMos(std::vector<std::string>& mosSequence, 
                                const std::vector<std::string>& gateSequence) {
    auto itGate = gateSequence.begin();
    for (size_t i = 1; itGate != gateSequence.end(); ++i, ++itGate) {
        size_t insertPos = i + (i - 1);
        if (insertPos <= mosSequence.size()) {
            mosSequence.insert(mosSequence.begin() + insertPos, *itGate);
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

std::vector<std::string> StandardCell::FindGateSequence(std::unordered_map<std::string, 
                                                    std::unordered_map<std::string, std::vector<std::string>>> SDToGate, 
                                                    const std::vector<std::string>& path){
    
    std::vector<std::string> gateSequence;
    for (auto it = path.begin(); it != path.end(); ++it) {
        const std::string& currentNode = *it;
        std::string nextNode;
        
        // 检查是否有下一个节点
        auto nextIt = std::next(it);
        if (nextIt != path.end()) {
            nextNode = *nextIt; // 获取下一个节点
        } else {
            // 没有下一个节点，可能需要处理边界情况
            break;
        }

        // 使用 currentNode 和 nextNode
        if(SDToGate[currentNode][nextNode].size() > 0 ){
            gateSequence.push_back(SDToGate[currentNode][nextNode][0]);
            SDToGate[currentNode][nextNode].erase(SDToGate[currentNode][nextNode].begin());
        }
        else if( SDToGate[nextNode][currentNode].size() > 0 ){
            gateSequence.push_back(SDToGate[nextNode][currentNode][0]);
            SDToGate[nextNode][currentNode].erase(SDToGate[nextNode][currentNode].begin());
        }
    }

    std::cout << " -------- Find Gate Sequence --------\n";
    return gateSequence;
}

bool containsString(const std::unordered_map<std::string, std::vector<std::array<std::string, 2>>>& GateToSD, 
                    const std::string& key, 
                    const std::string& searchString) {
    auto it = GateToSD.find(key);
    if (it != GateToSD.end()) {
        const auto& arrays = it->second;
        for (const auto& arr : arrays) {
            if (arr[0] == searchString || arr[1] == searchString) {
                return true; // 找到了字符串
            }
        }
    }
    return false; // 没找到字符串
}

int isEulerian(const std::unordered_map<std::string, size_t>& Nodes){
    int odd = 0;
    for(auto& node : Nodes){
        if(node.second % 2 == 1){
            odd++;
        }
    }
    if(odd == 0){
        return 1;
    }
    else if(odd == 2){
        return 2;
    }
    else{
        return 0;
    }
}

std::vector<std::string> StandardCell::FindMosSequence(std::unordered_map<std::string, std::vector<std::array<std::string, 2>>> GateToSD, 
                                                    const std::vector<std::string>& gateSequence, 
                                                    const std::unordered_map<std::string, size_t>& Nodes){

    std::vector<std::string> mosSequence;
    int eulerian = isEulerian(Nodes);
    /* gateSequence guarantee each edges */
    /* check start and end point */
    std::string startgate = gateSequence.front();
    std::string endgate = gateSequence.back();
    std::string startnode;
    std::string endnode;
    if(eulerian == 2){
        /* start and end point are the different */
        for(auto& node : Nodes){
            if(node.second % 2 == 1){
                if(containsString(GateToSD, startgate, node.first)){
                    startnode = node.first;
                }
                else if(containsString(GateToSD, endgate, node.first)){
                    endnode = node.first;
                }
            }
        }   
    }
    else if(eulerian == 1){
        /* start and end point are same */
        startnode = GateToSD[startgate][0][0];
        endnode = startnode;
    }
    else{
        /* no eulerian path */
        std::cout << " -------- No Eulerian Path --------\n";
        return mosSequence;
    }

    /* check continuity */
    std::string curnode = startnode;
    std::string gate;
    for(auto it = gateSequence.begin(); it != gateSequence.end(); ++it){
        gate = *it;
        // auto nextgateIt = std::next(it);
        if(containsString(GateToSD, gate, curnode)){
            /* find next node */
            mosSequence.push_back(curnode);
            for(auto& node : GateToSD[gate]){
                if(node[0] == curnode){
                    curnode = node[1];
                    break;
                }
                else if(node[1] == curnode){
                    curnode = node[0];
                    break;
                }
            }
            mosSequence.push_back(gate);
        }
        else{
            /* gateSequence not for this mos*/
            std::cout << " -------- No Eulerian Path --------\n";
        }
    }
    mosSequence.push_back(curnode);
    std::cout << " -------- Find Mos Sequence --------\n";
    return mosSequence;
}

/* workflow */
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

void StandardCell::FINFETsToGraph(){
    /* count outgoing & incoming */
    for(size_t i = 0; i < FINFETs.size(); i++){
        
        if(FINFETs[i].type == "nmos_rvt"){
            /*  nmos, d->s */
            nmosGraph.Nodes_[FINFETs[i].source] += 1;
            nmosGraph.Nodes_[FINFETs[i].drain] += 1;

            nmosGraph.SDToGate_[FINFETs[i].source][FINFETs[i].drain].push_back(FINFETs[i].gate);

            nmosGraph.GateToSD_[FINFETs[i].gate].push_back({FINFETs[i].source, FINFETs[i].drain});

            nmosGraph.adjMatrix[FINFETs[i].source][FINFETs[i].drain] += 1;
            nmosGraph.adjMatrix[FINFETs[i].drain][FINFETs[i].source] += 1;
        }
        else if(FINFETs[i].type == "pmos_rvt"){
            /* pmos, s->d */
            pmosGraph.Nodes_[FINFETs[i].source] += 1;
            pmosGraph.Nodes_[FINFETs[i].drain] += 1;

            pmosGraph.SDToGate_[FINFETs[i].source][FINFETs[i].drain].push_back(FINFETs[i].gate);

            pmosGraph.GateToSD_[FINFETs[i].gate].push_back({FINFETs[i].source, FINFETs[i].drain});

            pmosGraph.adjMatrix[FINFETs[i].source][FINFETs[i].drain] += 1;
            pmosGraph.adjMatrix[FINFETs[i].drain][FINFETs[i].source] += 1;
        }
    }
    std::cout << " -------- Graph Done --------\n";
}

void StandardCell::generateStickDiagram(){

    pmosSequence_ = pmosGraph.FindEulerianPath();
    gateSequence_ = FindGateSequence(pmosGraph.SDToGate_, pmosSequence_);
    nmosSequence_ = FindMosSequence(nmosGraph.GateToSD_, gateSequence_, nmosGraph.Nodes_);
    mergeIntoMos(pmosSequence_, gateSequence_);

    // nmosSequence_ = nmosGraph.FindEulerianPath();
    // gateSequence_ = FindGateSequence(nmosGraph.SDToGate_, nmosSequence_);
    // pmosSequence_ = FindMosSequence(pmosGraph.GateToSD_, gateSequence_, pmosGraph.Nodes_);
    // mergeIntoMos(nmosSequence_, gateSequence_);

    if(nmosSequence_.size() != 0 && pmosSequence_.size() != 0)
        std::cout << " -------- Stick Diagram Done --------\n";
    else
        std::cout << " -------- Stick Diagram Fail --------\n";
}