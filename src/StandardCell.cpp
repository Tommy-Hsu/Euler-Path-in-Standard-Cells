#include <iostream>
#include <fstream>
#include <algorithm>
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

void Graph::reset(){
    this->Nodes_.clear();
    this->SDToGate_.clear();
    this->GateToSD_.clear();
    this->adjMatrix.clear();
}

/* function */
std::string Graph::startEulerian()
{
    // Check if all non-zero degree vertices are connected

    int odd = 0;
    std::string oddNodeKey;
    std::vector<std::string> oddKeys;
    for(auto& node : this->Nodes_){
        if(node.second % 2 == 1){
            odd++;
            oddKeys.push_back(node.first);
        }
    }
    /* 
        decide to add "a" dummy node & "some" dummy edges 
        guarantee connected graph (only one component)
    */
    if (odd > 2) {
        /* add a dummy node and add dummy edges */
        while(oddKeys.size() > 2){
            int index1 = rand() % oddKeys.size();
            int index2;
            do {
                index2 = rand() % oddKeys.size();
            } while (index1 == index2);
            std::string node1 = oddKeys[index1];
            std::string node2 = oddKeys[index2];
            std::string dummyNode = "Dummy";

            /* Nodes_ */
            this->Nodes_[dummyNode] += 2;
            this->Nodes_[node1] += 1;
            this->Nodes_[node2] += 1;
            /* SDToGate_ */
            this->SDToGate_[dummyNode][node1].push_back("Dummy");
            this->SDToGate_[dummyNode][node2].push_back("Dummy");
            /* GateToSD_ */
            this->GateToSD_["Dummy"].push_back({dummyNode, node1});
            this->GateToSD_["Dummy"].push_back({dummyNode, node2});
            /* adjMatrix */
            this->adjMatrix[dummyNode][node1] += 1;
            this->adjMatrix[node1][dummyNode] += 1;
            this->adjMatrix[dummyNode][node2] += 1;
            this->adjMatrix[node2][dummyNode] += 1;
            /* erase */
            oddKeys.erase(oddKeys.begin() + index1);
            oddKeys.erase(oddKeys.begin() + index2 - (index2 > index1 ? 1 : 0));
        }
        oddNodeKey = oddKeys[rand() % oddKeys.size()];
        return oddNodeKey;
    } else if (odd == 2) {
        oddNodeKey = oddKeys[rand() % oddKeys.size()];
        return oddNodeKey;
    } else {
        size_t index = std::rand() % this->Nodes_.size();
        auto it = this->Nodes_.begin();
        std::advance(it, index);
        return it->first;
    }
}

std::vector<std::string> Graph::findEulerianPath()
{
    /* 
        assignment guarantee exit an euler path 
        by adding some dummy edge (gate) to the graph
    */
    std::string startpoint = startEulerian();
    std::unordered_map<std::string, std::unordered_map<std::string, size_t>> temp_adjMatrix = this->adjMatrix;
    std::stack<std::string> stack;
    std::vector<std::string> path;

    std::string cur = startpoint;

    while(!stack.empty() || temp_adjMatrix[cur].size() > 0){
        if(temp_adjMatrix[cur].size() == 0){
            path.push_back(cur);
            cur = stack.top();
            stack.pop();
        }
        else{
            stack.push(cur);
            auto it = temp_adjMatrix[cur].begin();
            std::advance(it, rand() % temp_adjMatrix[cur].size());
            std::string next = it->first;
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
    for (size_t i = 1; itGate != gateSequence.end(); ++itGate) {
        size_t insertPos = i + (i - 1);
        if(mosSequence[insertPos] == "Dummy"){
            ++i;
        }
        if (insertPos <= mosSequence.size() && *itGate != "Dummy") {
            mosSequence.insert(mosSequence.begin() + insertPos, *itGate);
            ++i;
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

std::vector<std::string> StandardCell::findGateSequence(std::unordered_map<std::string, 
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
        if (SDToGate[currentNode][nextNode].size() > 0) {
            int randomIndex = rand() % SDToGate[currentNode][nextNode].size();
            gateSequence.push_back(SDToGate[currentNode][nextNode][randomIndex]);
            SDToGate[currentNode][nextNode].erase(SDToGate[currentNode][nextNode].begin() + randomIndex);
        } else if (SDToGate[nextNode][currentNode].size() > 0) {
            int randomIndex = rand() % SDToGate[nextNode][currentNode].size();
            gateSequence.push_back(SDToGate[nextNode][currentNode][randomIndex]);
            SDToGate[nextNode][currentNode].erase(SDToGate[nextNode][currentNode].begin() + randomIndex);
        }
    }
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

std::vector<std::string> StandardCell::findMosSequence(std::unordered_map<std::string, std::vector<std::array<std::string, 2>>> GateToSD, 
                                                    const std::vector<std::string>& gateSequence, 
                                                    const std::unordered_map<std::string, size_t>& Nodes){

    std::vector<std::string> mosSequence;
    int eulerian = isEulerian(Nodes);
    /* gateSequence guarantee each edges */
    /* check start and end point */
    std::string startgate = gateSequence.front();
    std::string endgate = gateSequence.back();
    std::string startnode = "";
    std::string endnode = "";
    if(eulerian == 2){
        /* start and end point are the different */
        for(auto& node : Nodes){
            if(node.second % 2 == 1){
                if(startnode.empty() && containsString(GateToSD, startgate, node.first)){
                    startnode = node.first;
                }
                else if(endnode.empty() && containsString(GateToSD, endgate, node.first)){
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
    for(auto& gate : gateSequence){
        // auto nextgateIt = std::next(it);
        if(containsString(GateToSD, gate, curnode)){
            /* find next node */
            mosSequence.push_back(curnode);
            for(auto it = GateToSD[gate].begin(); it != GateToSD[gate].end(); ++it){
                if((*it)[0] == curnode){
                    curnode = (*it)[1];
                    GateToSD[gate].erase(it);
                    break;
                }
                else if((*it)[1] == curnode){
                    curnode = (*it)[0];
                    GateToSD[gate].erase(it);
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

std::vector<std::string> StandardCell::SequenceToFINFETs(std::vector<SPICE_FINFET> FINFETs, 
                                                        const std::vector<std::string>& sequence,
                                                        const std::string& type){
    std::vector<std::string> mosFINFETs;
    std::string targetType = type;
    std::cout << " -------- Find " << targetType << " --------\n";
    for (size_t i = 1; i < sequence.size() - 1; i+=2) {
        const std::string& left = sequence[i - 1];
        const std::string& gate = sequence[i];
        const std::string& right = sequence[i + 1];
        if( gate == "Dummy"){
            mosFINFETs.push_back("Dummy");
            continue;
        }

        auto it = FINFETs.begin();
        while (it != FINFETs.end()) {
            if (it->gate == gate && it->source == left && it->drain == right && it->type == targetType) {
                mosFINFETs.push_back(it->name.substr(1));
                it = FINFETs.erase(it); // 删除元素并更新迭代器
            } else if (it->gate == gate && it->source == right && it->drain == left && it->type == targetType) {
                mosFINFETs.push_back(it->name.substr(1));
                it = FINFETs.erase(it); // 删除元素并更新迭代器
            }else {
                ++it;
            }
        }
    }
    return mosFINFETs;
}

bool StandardCell::alignSequences(std::vector<std::string>& vec1, std::vector<std::string>& vec2) {
    size_t index1 = 0, index2 = 0;
    while (index1 < vec1.size() && index2 < vec2.size()) {
        if (vec1[index1] == "Dummy" && vec2[index2] != "Dummy") {
            vec2.insert(vec2.begin() + index2, "Dummy");
            vec2.insert(vec2.begin() + index2 + 1, "Dummy");
        } else if (vec2[index2] == "Dummy" && vec1[index1] != "Dummy") {
            vec1.insert(vec1.begin() + index1, "Dummy");
            vec1.insert(vec1.begin() + index1 + 1, "Dummy");
        } else if (vec1[index1] != vec2[index2]){
            return false;
        }
        
        // 確保只有當沒有插入 "Dummy" 時，索引才增加
        if (vec1[index1] == vec2[index2]) {
            index1++;
            index2++;
        }
    }
    return true;
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
        FINFETs_.push_back(FINFET);
    }
    
    input.close();
    std::cout << " -------- Parse Done --------\n";

}

void StandardCell::FINFETsToGraph(){
    nmosGraph.reset();
    pmosGraph.reset();
    /* count outgoing & incoming */
    for(size_t i = 0; i < FINFETs_.size(); i++){
        
        if(FINFETs_[i].type == "nmos_rvt"){
            /*  nmos, d->s */
            nmosGraph.Nodes_[FINFETs_[i].source] += 1;
            nmosGraph.Nodes_[FINFETs_[i].drain] += 1;

            nmosGraph.SDToGate_[FINFETs_[i].source][FINFETs_[i].drain].push_back(FINFETs_[i].gate);

            nmosGraph.GateToSD_[FINFETs_[i].gate].push_back({FINFETs_[i].source, FINFETs_[i].drain});

            nmosGraph.adjMatrix[FINFETs_[i].source][FINFETs_[i].drain] += 1;
            nmosGraph.adjMatrix[FINFETs_[i].drain][FINFETs_[i].source] += 1;
        }
        else if(FINFETs_[i].type == "pmos_rvt"){
            /* pmos, s->d */
            pmosGraph.Nodes_[FINFETs_[i].source] += 1;
            pmosGraph.Nodes_[FINFETs_[i].drain] += 1;

            pmosGraph.SDToGate_[FINFETs_[i].source][FINFETs_[i].drain].push_back(FINFETs_[i].gate);

            pmosGraph.GateToSD_[FINFETs_[i].gate].push_back({FINFETs_[i].source, FINFETs_[i].drain});

            pmosGraph.adjMatrix[FINFETs_[i].source][FINFETs_[i].drain] += 1;
            pmosGraph.adjMatrix[FINFETs_[i].drain][FINFETs_[i].source] += 1;
        }
        IOs_.insert(FINFETs_[i].source);
        IOs_.insert(FINFETs_[i].drain);
    }
}

bool StandardCell::generateStickDiagram(){

    NmosSequence_ = nmosGraph.findEulerianPath();
    NmosGateSequence_ = findGateSequence(nmosGraph.SDToGate_, NmosSequence_);
    PmosSequence_ = pmosGraph.findEulerianPath();
    PmosGateSequence_ = findGateSequence(pmosGraph.SDToGate_, PmosSequence_);
    // PmosSequence_ = FindMosSequence(pmosGraph.GateToSD_, gateSequence_, pmosGraph.Nodes_);
    
    return alignSequences(NmosGateSequence_, PmosGateSequence_);
}

void StandardCell::SequenceToPins(){

    mergeIntoMos(NmosSequence_, NmosGateSequence_);
    mergeIntoMos(PmosSequence_, PmosGateSequence_);
    pmosFINFETs_ = SequenceToFINFETs(FINFETs_, PmosSequence_, "pmos_rvt");
    nmosFINFETs_ = SequenceToFINFETs(FINFETs_, NmosSequence_, "nmos_rvt");
}

void StandardCell::calculateHPWL(){
    HPWL_ = 0.f;
    /* Height */
    float rowgap = 27.f;
    float pmosHeight = 0.f;
    float nmosHeight = 0.f;
    for(auto& FIN : FINFETs_){
        if(FIN.type == "pmos_rvt"){
            pmosHeight = FIN.width;
        }
        else if(FIN.type == "nmos_rvt"){
            nmosHeight = FIN.width;
        }
    }
    /* Width */
    float gateWidth = 20.f;
    float srctgtWidth = 25.f;
    float midWidth = 34.f;
    /* Calculate */
    for(auto& IO : IOs_){
        /* x */
        float x = 0.f;
        size_t startIndex = 0;
        size_t endIndex = 0;
        for(size_t i = 0; i < PmosSequence_.size(); i++){
            if(PmosSequence_[i] == IO || NmosSequence_[i] == IO){
                startIndex = i;
                break;
            }
        }
        for(size_t i = PmosSequence_.size() - 1; i >= 0; i--){
            if(PmosSequence_[i] == IO || NmosSequence_[i] == IO){
                endIndex = i;
                break;
            }
        }

        if( startIndex == endIndex)
            x = 0.f;
        else{
            for(size_t i = startIndex+1; i < endIndex; i++){
                if(i%2 == 1)
                    x += gateWidth;
                else
                    x += midWidth;
            }
            x += (startIndex == 0) ? (srctgtWidth/2.0) : (midWidth/2.0);
            x += (endIndex == PmosSequence_.size() - 1) ? (srctgtWidth/2.0) : (midWidth/2.0);
        }
        /* y */
        float y = (std::find(PmosSequence_.begin(), PmosSequence_.end(), IO) != PmosSequence_.end() &&
                std::find(NmosSequence_.begin(), NmosSequence_.end(), IO) != NmosSequence_.end()) ? (pmosHeight/2.0 + rowgap + nmosHeight/2.0): 0.f;
        HPWL_ += (x + y);
    }
    std::cout << " -------- Calculate HPWL --------\n";
}

void StandardCell::outputResult(std::ofstream& output){
    
    output << HPWL_ << std::endl;
    for (auto& FINFET : pmosFINFETs_) {
        output << FINFET << " ";
    }output << std::endl;
    for (auto pmos : PmosSequence_)
    {
        output << pmos << " ";
    }output << std::endl;
    for (auto& FINFET : nmosFINFETs_) {
        output << FINFET << " ";
    }output << std::endl;
    for (auto nmos : NmosSequence_)
    {
        output << nmos << " ";
    }

    output.close();
    std::cout << " -------- Output Done --------\n";
}