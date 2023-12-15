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
std::vector<std::string> StandardCell::mergeIntoMos(const std::vector<std::string>& mosSequence, 
                                const std::vector<std::string>& gateSequence) {
    std::vector<std::string> ResultSequence;
    size_t mosIndex = 0;
    size_t gateIndex = 0;
    while (mosIndex < mosSequence.size() && gateIndex < gateSequence.size()) {
        if(gateSequence[gateIndex] == "Dummy"){
            ResultSequence.push_back(mosSequence[mosIndex]);
            ResultSequence.push_back("Dummy");
            mosIndex += 2;
            gateIndex += 2;
        }
        else{
            ResultSequence.push_back(mosSequence[mosIndex]);
            ResultSequence.push_back(gateSequence[gateIndex]);
            mosIndex++;
            gateIndex++;
        }
    }
    ResultSequence.push_back(mosSequence[mosIndex]);
    return ResultSequence;
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

std::string findNextNodeinPairGate(std::string firstgate, std::string secondgate,
                                std::unordered_map<std::string, std::vector<std::array<std::string, 2>>>& GateToSD,
                                const std::unordered_map<std::string, size_t>& Nodes,
                                std::vector<std::string>& mosSequence ){
                                    
    std::string startleftnode = "";
    std::string startmiddlenode = "";
    std::string startrightnode = "";

    for(auto& node : Nodes){
        if(containsString(GateToSD, firstgate, node.first) && containsString(GateToSD, secondgate, node.first)){
            startmiddlenode = node.first;

            std::vector<std::vector<std::array<std::string, 2>>::iterator> candidateIterators;
            for(auto it = GateToSD.at(firstgate).begin(); it != GateToSD.at(firstgate).end(); ++it){
                if((*it)[0] == startmiddlenode || (*it)[1] == startmiddlenode)
                    candidateIterators.push_back(it);
            }
            int randomIndex = std::rand() % candidateIterators.size();
            auto chosenIt = candidateIterators[randomIndex];
            startleftnode = (*chosenIt)[(*chosenIt)[0] == startmiddlenode ? 1 : 0];
            GateToSD.at(firstgate).erase(chosenIt);
            candidateIterators.clear();

            for(auto it = GateToSD.at(secondgate).begin(); it != GateToSD.at(secondgate).end(); ++it){
                if((*it)[0] == startmiddlenode || (*it)[1] == startmiddlenode)
                    candidateIterators.push_back(it);
            }
            randomIndex = std::rand() % candidateIterators.size();
            chosenIt = candidateIterators[randomIndex];
            startrightnode = (*chosenIt)[(*chosenIt)[0] == startmiddlenode ? 1 : 0];
            GateToSD.at(secondgate).erase(chosenIt);
            mosSequence.push_back(startleftnode);
            mosSequence.push_back(startmiddlenode);
            break;
        }
    }

    if(startleftnode == "" && startmiddlenode == "" && startrightnode == ""){
        int randomIndex = std::rand() % GateToSD.at(firstgate).size();
        startleftnode = GateToSD.at(firstgate)[randomIndex][0];
        startmiddlenode = GateToSD.at(firstgate)[randomIndex][1];
        GateToSD.at(firstgate).erase(GateToSD.at(firstgate).begin() + randomIndex);
        startrightnode = "Dummy";
        mosSequence.push_back(startleftnode);
        mosSequence.push_back(startmiddlenode);
    }
    return startrightnode;
}

std::vector<std::string> StandardCell::findMosSequence(std::unordered_map<std::string, std::vector<std::array<std::string, 2>>> GateToSD, 
                                                    std::vector<std::string>& gateSequence, 
                                                    const std::unordered_map<std::string, size_t>& Nodes){

    std::vector<std::string> mosSequence;
    int eulerian = isEulerian(Nodes);
    /* gateSequence guarantee each edges */
    /* check start and end point */
    std::string firstgate = gateSequence[0];
    std::string secondgate = gateSequence[1];

    /* check continuity */
    std::string curnode = findNextNodeinPairGate(firstgate, secondgate, GateToSD, Nodes, mosSequence);
    auto it = (curnode == "Dummy")? gateSequence.begin() + 1: gateSequence.begin() +2;
    for(; it != gateSequence.end(); ++it){
        if(curnode != "Dummy") mosSequence.push_back(curnode);
        if(containsString(GateToSD, *it, curnode)){
            /* find next node */
            std::vector<decltype(GateToSD[*it].begin())> candidates;  // 存储符合条件的迭代器
            for(auto it2 = GateToSD[*it].begin(); it2 != GateToSD[*it].end(); ++it2){
                if((*it2)[0] == curnode || (*it2)[1] == curnode){
                    candidates.push_back(it2);
                }
            }
            auto chosenIt = candidates[rand()%candidates.size()]; // 随机选择一个候选迭代器
            curnode = ((*chosenIt)[0] == curnode) ? (*chosenIt)[1] : (*chosenIt)[0];
            GateToSD[*it].erase(chosenIt);      
        }
        else{
            if(*it != "Dummy")
                it = gateSequence.insert(it, "Dummy");
            ++it;
            if(*it != "Dummy")
                it = gateSequence.insert(it, "Dummy");
            ++it;
            curnode = "Dummy";
        }

        if(curnode == "Dummy"){
            mosSequence.push_back("Dummy");
            if(std::next(it) == gateSequence.end()){
                mosSequence.push_back(GateToSD[*it][0][0]);
                mosSequence.push_back(GateToSD[*it][0][1]);
                break;
            }
            else{
                firstgate = *it;
                secondgate = *(it + 1);
                curnode = findNextNodeinPairGate(firstgate, secondgate, GateToSD, Nodes, mosSequence);
                it = (curnode == "Dummy")? it : it + 1;
            }

        }
    }
    if(curnode != "Dummy")
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
    while(index1 < vec1.size() && index2 < vec2.size()){
        if(vec1[index1] == "Dummy" && vec2[index2] != "Dummy"){
            vec2.insert(vec2.begin() + index2, "Dummy");
            vec2.insert(vec2.begin() + index2 + 1, vec2[index2 - 1]);
        }
        else if(vec2[index2] == "Dummy" && vec1[index1] != "Dummy"){
            vec1.insert(vec1.begin() + index1, "Dummy");
            vec1.insert(vec1.begin() + index1 + 1, vec1[index1 - 1]);
        }
        else{
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
    PmosGateSequence_ = NmosGateSequence_;
    PmosSequence_ = findMosSequence(pmosGraph.GateToSD_, PmosGateSequence_, pmosGraph.Nodes_);
    
    NmosResult_ = mergeIntoMos(NmosSequence_, NmosGateSequence_);
    PmosResult_ = mergeIntoMos(PmosSequence_, PmosGateSequence_);
    alignSequences(NmosResult_, PmosResult_);

    return (NmosResult_.size() == PmosResult_.size());
}

void StandardCell::SequenceToPins(){

    pmosFINFETs_ = SequenceToFINFETs(FINFETs_, PmosResult_, "pmos_rvt");
    nmosFINFETs_ = SequenceToFINFETs(FINFETs_, NmosResult_, "nmos_rvt");
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
        for(size_t i = 0; i < PmosResult_.size(); i++){
            if((PmosResult_[i] == IO || NmosResult_[i] == IO) && i%2 == 0){
                startIndex = i;
                break;
            }
        }
        for(size_t i = PmosResult_.size() - 1; i >= 0; i--){
            if((PmosResult_[i] == IO || NmosResult_[i] == IO) && i%2 == 0){
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
            x += (endIndex == PmosResult_.size() - 1) ? (srctgtWidth/2.0) : (midWidth/2.0);
        }
        /* y */
        auto it = std::find(PmosResult_.begin(), PmosResult_.end(), IO);
        bool isInPmos = (it != PmosResult_.end());
        bool isInNmos = (std::find(NmosResult_.begin(), NmosResult_.end(), IO) != NmosResult_.end());
        bool isEvenIndexInPmos = isInPmos && (std::distance(PmosResult_.begin(), it) % 2 == 0);
        float y = (isInPmos && isInNmos && isEvenIndexInPmos) ? (pmosHeight/2.0 + rowgap + nmosHeight/2.0) : 0.f;
        
        HPWL_ += (x + y);
    }
    std::cout << " -------- Calculate HPWL --------\n";
}

void StandardCell::outputResult(std::ofstream& output){
    
    output << HPWL_ << std::endl;
    for (auto& FINFET : pmosFINFETs_) {
        output << FINFET << " ";
    }output << std::endl;
    for (auto pmos : PmosResult_)
    {
        output << pmos << " ";
    }output << std::endl;
    for (auto& FINFET : nmosFINFETs_) {
        output << FINFET << " ";
    }output << std::endl;
    for (auto nmos : NmosResult_)
    {
        output << nmos << " ";
    }

    output.close();
    std::cout << " -------- Output Done --------\n";
}