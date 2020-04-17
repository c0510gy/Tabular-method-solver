/*

title: TabularMethodSolver.h
author: 윤상건 (Sang-geon Yun, ggj06281@kookmin.ac.kr)

*/
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <cmath>
#include <stack>
#include <queue>
#include <set>
#include <map>
#include "MaxSegmentTree.h"
#define MAX_INPUTS 64
using namespace std;

typedef unsigned long long int ull;

class ImcntBits{
private:
    ull bits;
    vector<bool> ask;
    int numberOfInputs;
public:
    ImcntBits();
    ImcntBits(int numberOfInputs, ull bits);
    string toString();
    int hammingDist(const ImcntBits& another);
    ImcntBits returnCombined(const ImcntBits& another);
    friend ImcntBits;
};
class Impcnt{
private:
    vector<ull> terms;
    ImcntBits bits;
    int numberOfInputs;
    bool check, isEPI;
public:
    Impcnt(int numberOfInputs);
    Impcnt(int numberOfInputs, ull term);
    Impcnt(const vector<ull>& terms);
    ull getTerm(int idx);
    int getNumberOfTerms();
    void addTerm(ull term);
    void setCheck();
    bool getCheck();
    void setEPI();
    bool getEPI();
    void printTerms();
    Impcnt returnCombined(const Impcnt& another);
    ImcntBits getBits();
    friend Impcnt;
};
class Tabular{
private:
    vector<ull> minterms, dontcares;
    vector<Impcnt> PIs;
    int numberOfInputs, totNumberOfEPI;
    void addMin(ull m);
    void addDon(ull d);
    int countBit(ull b);
public:
    Tabular();
    Tabular(vector<ull>& mins, vector<ull>& donts);
    void addMinterm(ull m);
    void addDontcare(ull d);
    void getPI();
    void getEPI();
    void buildGraph(vector<vector<int>>& GL, vector<vector<int>>& GR, vector<int>& rev);
    void bruteForce(vector<vector<int>>& G, vector<int>& selected, vector<int>& nowCase, vector<int>& minCase, int selCnt, int idx, int cost, int& minCost);
    void greedy(vector<vector<int>>& GL, vector<vector<int>>& GR, vector<int>& selected, vector<int>& minCase);
    vector<int> approximationSolver();
    vector<int> trueSolver();
    string ansToString(vector<int> ans);
    string solve(bool approx);
    int solveTest(bool approx);
};
struct segNode{
    int value, idx;
    segNode(){
        value = -1; idx = -1;
    }
    segNode(int value, int idx){
        this->value = value;
        this->idx = idx;
    }
    bool operator<(const segNode& another)const{
        return value < another.value;
    }
    bool operator>(const segNode& another)const{
        return value > another.value;
    }
    segNode operator+(const segNode& another)const{
        segNode ret;
        ret.value = value + another.value;
        ret.idx = idx;
        return ret;
    }
};

ImcntBits::ImcntBits(){
    numberOfInputs = MAX_INPUTS;
    ask.resize(MAX_INPUTS, false);
}
ImcntBits::ImcntBits(int numberOfInputs, ull bits){
    this->numberOfInputs = numberOfInputs;
    this->bits = bits;
    ask.resize(numberOfInputs, false);
}
string ImcntBits::toString(){
    string str = "";
    stack<char> stk;
    for(ull t = bits, i = 0; i < numberOfInputs; t >>= 1, ++i){
        if(ask[i])
            stk.push('-');
        else{
            stk.push((t & 1) + '0');
        }
    }
    while(!stk.empty()){
        str += stk.top();
        stk.pop();
    }
    return str;
}
int ImcntBits::hammingDist(const ImcntBits& another){
    int dist = 0;
    ull diff = bits ^ another.bits;
    for(int i = 0; i < numberOfInputs; diff >>= 1, ++i){
        if(ask[i] != another.ask[i])
            return -1;
        if(ask[i]) continue;
        dist += (diff & 1);
    }
    return dist;
}
ImcntBits ImcntBits::returnCombined(const ImcntBits& another){
    ImcntBits ret(numberOfInputs, bits);
    ull diff = bits ^ another.bits;
    for(int i = 0; i < numberOfInputs; diff >>= 1, ++i){
        ret.ask[i] = ask[i];
        if(diff & 1)
            ret.ask[i] = true;
    }
    return ret;
}

Impcnt::Impcnt(int numberOfInputs){
    this->numberOfInputs = numberOfInputs;
    check = false;
    isEPI = false;
}
Impcnt::Impcnt(int numberOfInputs, ull term){
    this->numberOfInputs = numberOfInputs;
    bits = ImcntBits(numberOfInputs, term);
    terms.push_back(term);
    check = false;
    isEPI = false;
}
Impcnt::Impcnt(const vector<ull>& terms){
    for(auto t: terms)
        this->terms.push_back(t);
    check = false;
    isEPI = false;
}
ull Impcnt::getTerm(int idx){
    return terms[idx];
}
int Impcnt::getNumberOfTerms(){
    return terms.size();
}
void Impcnt::addTerm(ull term){
    terms.push_back(term);
}
void Impcnt::setCheck(){
    check = true;
}
bool Impcnt::getCheck(){
    return check;
}
void Impcnt::setEPI(){
    isEPI = true;
}
bool Impcnt::getEPI(){
    return isEPI;
}
void Impcnt::printTerms(){
    for(int i = 0; i < terms.size(); ++i)
        cout << terms[i] << ", ";
    cout << endl;
}
Impcnt Impcnt::returnCombined(const Impcnt& another){
    Impcnt ret(numberOfInputs);
    for(int i = 0, j = 0; i < terms.size() || j < another.terms.size(); ){
        if(i < terms.size() && j < another.terms.size()){
            if(terms[i] > another.terms[j])
                ret.addTerm(another.terms[j++]);
            else
                ret.addTerm(terms[i++]);
        }else if(i < terms.size())
            ret.addTerm(terms[i++]);
        else
            ret.addTerm(another.terms[j++]);
    }
    ret.bits = bits.returnCombined(another.bits);
    return ret;
}
ImcntBits Impcnt::getBits(){
    return bits;
}

void Tabular::addMin(ull m){
    minterms.push_back(m);
    numberOfInputs = max(numberOfInputs, (int)log2(m) + 1);
}
void Tabular::addDon(ull d){
    dontcares.push_back(d);
    numberOfInputs = max(numberOfInputs, (int)log2(d) + 1);
}
int Tabular::countBit(ull b){
    int cnt = 0;
    for(; b; b >>= 1) cnt += (b & 1);
    return cnt;
}
Tabular::Tabular(){
    numberOfInputs = 0;
}
Tabular::Tabular(vector<ull>& mins, vector<ull>& donts){
    numberOfInputs = 0;
    for(auto m: mins)
        addMin(m);
    for(auto d: donts)
        addDon(d);
}
void Tabular::addMinterm(ull m){
    addMin(m);
}
void Tabular::addDontcare(ull d){
    addDon(d);
}
void Tabular::getPI(){
    vector<vector<vector<Impcnt>>> lists;
    vector<vector<Impcnt>> firstList(numberOfInputs + 1);
    for(int i = 0; i < minterms.size(); ++i)
        firstList[countBit(minterms[i])].push_back(Impcnt(numberOfInputs, minterms[i]));
    for(int i = 0; i < dontcares.size(); ++i)
        firstList[countBit(dontcares[i])].push_back(Impcnt(numberOfInputs, dontcares[i]));
    lists.push_back(firstList);
    for(int i = 1; ; ++i){
        vector<vector<Impcnt>> nList(numberOfInputs + 1);
        vector<vector<Impcnt>>& lList = lists.back();

        bool changed = false;
        
        for(int j = 0; j < numberOfInputs; ++j){ // Number of nums
            set<string> duplicates;
            for(int l = 0; l < lList[j].size(); ++l){
                for(int n = 0; n < lList[j + 1].size(); ++n){
                    int dist = lList[j][l].getBits().hammingDist(lList[j + 1][n].getBits());
                    if(dist == 1){ // Combine
                        lList[j][l].setCheck();
                        lList[j + 1][n].setCheck();

                        Impcnt newCircle = lList[j][l].returnCombined(lList[j + 1][n]);
                        string hash = newCircle.getBits().toString();
                        if(duplicates.find(hash) == duplicates.end()){
                            duplicates.insert(hash);

                            nList[j].push_back(newCircle);
                            changed = true;
                        }
                    }
                }
            }
        }
        if(!changed)
            break;
        lists.push_back(nList);
    }
    PIs.clear();
    for(int i = 0; i < lists.size(); ++i)
        for(int j = 0; j <= numberOfInputs - i; ++j)
            for(int k = 0; k < lists[i][j].size(); ++k)
                if(!lists[i][j][k].getCheck())
                    PIs.push_back(lists[i][j][k]);
}
void Tabular::bruteForce(vector<vector<int>>& G, vector<int>& selected, vector<int>& nowCase, vector<int>& minCase , int selCnt, int idx, int cost, int& minCost){
    if(idx == G.size() || (minCost != -1 && minCost <= cost))
        return;
    bruteForce(G, selected, nowCase, minCase, selCnt, idx + 1, cost, minCost);
    int cnt = 0;
    for(int i = 0; i < G[idx].size(); ++i){
        int u = G[idx][i];
        if(!selected[u]) ++cnt;
        ++selected[u];
    }
    nowCase.push_back(idx);
    if(cnt){
        if(selCnt + cnt == selected.size()){
            if(minCost == -1 || minCost > cost + 1){
                minCost = cost + 1;
                minCase.clear();
                for(int i = 0; i < nowCase.size(); ++i)
                    minCase.push_back(nowCase[i]);
            }
        }else{
            bruteForce(G, selected, nowCase, minCase, selCnt + cnt, idx + 1, cost + 1, minCost);
        }
    }
    for(int i = 0; i < G[idx].size(); ++i){
        int u = G[idx][i];
        --selected[u];
    }
    nowCase.pop_back();
}
void Tabular::greedy(vector<vector<int>>& GL, vector<vector<int>>& GR, vector<int>& selected, vector<int>& minCase){
    int nL = GL.size(), nR = GR.size();
    MaxSegment<segNode> seg(nL);
    for(int v = 0; v < nL; ++v)
        seg.setValue(0, v, 0, nL - 1, segNode(GL[v].size(), v));

    int count = 0;
    while(count < selected.size()){
        segNode maxnode = seg.query(0, 0, nL - 1, 0, nL - 1);
        minCase.push_back(maxnode.idx);
        for(int i = 0; i < GL[maxnode.idx].size(); ++i){
            int u = GL[maxnode.idx][i];

            if(selected[u]) continue;
            selected[u] = true;
            ++count;
            for(int j = 0; j < GR[u].size(); ++j){
                int v = GR[u][j];

                seg.update(0, v, 0, nL - 1, segNode(-1, 0));
            }
        }
    }
}
void Tabular::getEPI(){
    // Coordinate compression
    set<ull> positions;
    map<ull, int> comp;
    for(int i = 0; i < minterms.size(); ++i)
        positions.insert(minterms[i]);
    int n = 0;
    for(auto itr = positions.begin(); itr != positions.end(); ++itr)
        comp[*itr] = n++;
    
    // Finding EPI
    totNumberOfEPI = 0;
    vector<int> cnt(n, 0), last(n, -1);
    for(int i = 0; i < PIs.size(); ++i){
        for(int j = 0; j < PIs[i].getNumberOfTerms(); ++j){
            ull term = PIs[i].getTerm(j);
            if(comp.find(term) == comp.end()) continue;
            int compPos = comp[term];
            ++cnt[compPos];
            last[compPos] = i;
        }
    }

    // Eliminating EPI
    for(int i = 0; i < minterms.size(); ++i){
        int compPos = comp[minterms[i]];
        if(cnt[compPos] == 1){
            if(PIs[last[compPos]].getEPI()) // Already EPI
                continue;
            PIs[last[compPos]].setEPI(); // EPI Found!
            ++totNumberOfEPI;
        }
    }
}
void Tabular::buildGraph(vector<vector<int>>& GL, vector<vector<int>>& GR, vector<int>& rev){
    // Coordinate compression
    set<ull> positions;
    map<ull, int> comp;
    for(int i = 0; i < minterms.size(); ++i)
        positions.insert(minterms[i]);
    
    // Eliminating coordinates that are satisfied by Coordinate compression
    set<ull> sat;
    for(int i = 0; i < PIs.size(); ++i){
        if(!PIs[i].getEPI()) // check EPI
            continue;
        for(int j = 0; j < PIs[i].getNumberOfTerms(); ++j)
            sat.insert(PIs[i].getTerm(j));
    }
    while(!sat.empty()){
        ull m = *sat.begin(); sat.erase(sat.begin());
        positions.erase(m);
    }
    int n = 0;
    for(auto itr = positions.begin(); itr != positions.end(); ++itr)
        comp[*itr] = n++;

    // Building Bipartite Graph to solve Set Cover problem
    int nA = PIs.size() - totNumberOfEPI;
    int nB = n;

    GL.resize(nA); GR.resize(nB);
    for(int i = 0, v = 0; i < PIs.size(); ++i){
        if(PIs[i].getEPI())
            continue;
        rev.push_back(i);
        for(int j = 0; j < PIs[i].getNumberOfTerms(); ++j){
            ull term = PIs[i].getTerm(j);
            if(comp.find(term) == comp.end()) continue;
            int u = comp[term];
            GL[v].push_back(u);
            GR[u].push_back(v);
        }
        ++v;
    }
}
vector<int> Tabular::approximationSolver(){
    vector<vector<int>> GL, GR;
    vector<int> rev, ans;
    buildGraph(GL, GR, rev);

    // Greedy
    vector<int> solCase, selected(GR.size(), 0);
    greedy(GL, GR, selected, solCase);

    for(int i = 0; i < solCase.size(); ++i)
        ans.push_back(rev[solCase[i]]);
    
    return ans;
}
vector<int> Tabular::trueSolver(){
    vector<vector<int>> GL, GR;
    vector<int> rev, ans;
    buildGraph(GL, GR, rev);

    // Brute force
    int minCost = -1;
    vector<int> nowCase, solCase, selected(GR.size(), 0);
    bruteForce(GL, selected, nowCase, solCase, 0, 0, 0, minCost);
    
    for(int i = 0; i < solCase.size(); ++i)
        ans.push_back(rev[solCase[i]]);
    
    return ans;
}
string Tabular::ansToString(vector<int> ans){
    string ansEq = "F = ";
    for(int i = 0; i < ans.size(); ++i){
        int j = ans[i];
        string ex = PIs[j].getBits().toString();
        for(int k = 0; k < ex.size(); ++k){
            if(ex[k] == '-') continue;
            ansEq += 'a' + k;
            if(ex[k] == '0')
                ansEq += '\'';
        }
        if(i < ans.size() - 1) ansEq += " + ";
    }
    return ansEq;
}
string Tabular::solve(bool approx){
    getPI();
    getEPI();
    vector<int> ans;
    if(approx)
        ans = approximationSolver();
    else
        ans = trueSolver();
    for(int i = 0, v = 0; i < PIs.size(); ++i)
        if(PIs[i].getEPI())
            ans.push_back(i);
    sort(ans.begin(), ans.end());
    string eq = ansToString(ans);
    return eq;
}
int Tabular::solveTest(bool approx){
    getPI();
    getEPI();
    vector<int> ans;
    if(approx)
        ans = approximationSolver();
    else
        ans = trueSolver();
    for(int i = 0, v = 0; i < PIs.size(); ++i)
        if(PIs[i].getEPI())
            ans.push_back(i);
    return ans.size();
}