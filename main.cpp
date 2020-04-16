#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <cmath>
#include <stack>
#include <queue>
#include <set>
#include <map>
#include "BiMatch.h"
#include "DinicWithLRFlow.h"
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

class Tabular{
private:
    vector<ull> minterms, dontcares;
    vector<Impcnt> PIs;
    int numberOfInputs;
    void addMin(ull m);
    void addDon(ull d);
    int countBit(ull b);
public:
    Tabular();
    Tabular(vector<ull>& mins, vector<ull>& donts);
    void addMinterm(ull m);
    void addDontcare(ull d);
    void getPI();
    void bruteForce(vector<vector<int>>& G, vector<int>& selected, vector<int>& nowCase, vector<int>& minCase , int selCnt, int idx, int cost, int& minCost);
    void solve();
};
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
                if(!lists[i][j][k].getCheck()){
                    PIs.push_back(lists[i][j][k]);
                    //cout << lists[i][j][k].getBits().toString() << endl;
                }
        
    cout << "Prime Implicants" << endl;
    for(int i = 0; i < PIs.size(); ++i){
        PIs[i].printTerms();
    }
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
void Tabular::solve(){
    // Coordinate compression
    set<ull> positions;
    map<ull, int> comp;
    for(int i = 0; i < minterms.size(); ++i)
        positions.insert(minterms[i]);
    int n = 0;
    for(auto itr = positions.begin(); itr != positions.end(); ++itr)
        comp[*itr] = n++;
    
    // Finding EPI
    int totNumberOfEPI = 0;
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

    // Eliminating EPI and re-compression
    set<ull> sat;
    for(int i = 0; i < minterms.size(); ++i){
        int compPos = comp[minterms[i]];
        if(cnt[compPos] == 1){
            if(PIs[last[compPos]].getEPI()) // Already EPI
                continue;
            PIs[last[compPos]].setEPI(); // EPI Found!
            ++totNumberOfEPI;
            for(int j = 0; j < PIs[last[compPos]].getNumberOfTerms(); ++j)
                sat.insert(PIs[last[compPos]].getTerm(j));
        }
    }
    while(!sat.empty()){
        ull m = *sat.begin(); sat.erase(sat.begin());
        positions.erase(m);
    }
    n = 0;
    comp.clear();
    for(auto itr = positions.begin(); itr != positions.end(); ++itr)
        comp[*itr] = n++;

    // Building Bipartite Graph to solve Set Cover problem
    int nA = PIs.size() - totNumberOfEPI;
    int nB = n;
    vector<vector<int>> biG;
    vector<int> selected(nB, 0), rev, ans;
    biG.resize(nA);
    cout << "Essential Prime Implicants" << endl;
    for(int i = 0, v = 0; i < PIs.size(); ++i){
        if(PIs[i].getEPI()){
            ans.push_back(i);
            PIs[i].printTerms();
            continue;
        }
        //PIs[i].printTerms();
        rev.push_back(i);
        for(int j = 0; j < PIs[i].getNumberOfTerms(); ++j){
            ull term = PIs[i].getTerm(j);
            if(comp.find(term) == comp.end()) continue;
            int u = comp[term];
            biG[v].push_back(u);
            //cout << v << "->" << u + nA << endl;
        }
        ++v;
    }

    // Brute force
    int minCost = -1;
    vector<int> nowCase, solCase;
    bruteForce(biG, selected, nowCase, solCase, 0, 0, 0, minCost);

    cout << "Minimum Cost: " << minCost << endl;
    for(int i = 0; i < solCase.size(); ++i)
        ans.push_back(rev[solCase[i]]);
        
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
    cout << ansEq << endl;

    /*
    LRMaxFlow lrflow(nA * 2 + nB + 3, 0, nA * 2 + nB + 2);
    int D = 0;
    lrflow.addEdge(0, 1, D, 0); // S - d -> S'
    for(int v = 0; v < nA; ++v){
        lrflow.addEdge(1, v + 2, 1, 0); // S' - 1 -> V
        lrflow.addEdge(0, v + 2, biG[v].size() - 1, 0); // S - (l - 1) -> V
        lrflow.addEdge(v + 2, nA + v + 2, biG[v].size(), biG[v].size()); // V - l / l -> V'
        for(int i = 0; i < biG[v].size(); ++i){
            int u = biG[v][i];
            lrflow.addEdge(nA + v + 2, nA * 2 + u + 2, 1, 0); // V' - 1 - > U
        }
    }
    for(int u = 0; u < nB; ++u)
        lrflow.addEdge(nA * 2 + u + 2, nA * 2 + nB + 2, 1, 0); // U - 1 -> T
    
    int maxFlow = lrflow.getMaxflow();
    int sum = 0;
    for(int i = 0; i < nA * 2 + nB + 3; i++) sum += lrflow.inSum[i];
    if(maxFlow != sum){
        cout << "NO\n";
    }
    cout << maxFlow << ", " << sum << endl;
    cout << sum - maxFlow << endl;
        
    //HopcroftKarp bimatch(biG);
        
    //bimatch.flow();
    int maxFlow2 = bi.solve();
    cout << maxFlow2 << endl;

    cout << maxFlow << endl << totNumberOfEPI << endl;
    cout << maxFlow + totNumberOfEPI << endl;
    */
}

int main(){
    freopen("./testCases.txt", "r", stdin);
    int tcase; cin >> tcase;
    while(tcase--){
        vector<ull> minterms, dontcares;
        int m, d;
        cin >> m;
        cout << "Minterms: ";
        for(int i = 0; i < m; ++i){
            ull t; cin >> t;
            minterms.push_back(t);
            cout << t << ", ";
        }
        cin >> d;
        cout << endl << "Dont cares: ";
        for(int i = 0; i < d; ++i){
            ull t; cin >> t;
            dontcares.push_back(t);
            cout << t << ", ";
        }
        cout << endl;
        Tabular tabular(minterms, dontcares);
        tabular.getPI();
        tabular.solve();
        cout << endl;
    }
    
    return 0;
}