#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <cmath>
#include <stack>
#include <set>
#define MAX_INPUTS 64
using namespace std;

typedef unsigned long long int ull;

class ImcntBits{
private:
    ull bits;
    vector<bool> ask;
    int numberOfInputs;
public:
    ImcntBits(){
        numberOfInputs = MAX_INPUTS;
        ask.resize(MAX_INPUTS, false);
    }
    ImcntBits(int numberOfInputs, ull bits){
        this->numberOfInputs = numberOfInputs;
        this->bits = bits;
        ask.resize(numberOfInputs, false);
    }
    string toString(){
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
    int hammingDist(const ImcntBits& another){
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
    ImcntBits returnCombined(const ImcntBits& another){
        ImcntBits ret(numberOfInputs, bits);
        ull diff = bits ^ another.bits;
        for(int i = 0; i < numberOfInputs; diff >>= 1, ++i){
            ret.ask[i] = ask[i];
            if(diff & 1)
                ret.ask[i] = true;
        }
        return ret;
    }
    friend ImcntBits;
};
class Impcnt{
private:
    vector<ull> terms;
    ImcntBits bits;
    int numberOfInputs;
    bool check, isEPI;
public:
    Impcnt(int numberOfInputs){
        this->numberOfInputs = numberOfInputs;
        check = false;
        isEPI = false;
    }
    Impcnt(int numberOfInputs, ull term){
        this->numberOfInputs = numberOfInputs;
        bits = ImcntBits(numberOfInputs, term);
        terms.push_back(term);
        check = false;
        isEPI = false;
    }
    Impcnt(const vector<ull>& terms){
        for(auto t: terms)
            this->terms.push_back(t);
        check = false;
        isEPI = false;
    }
    void addTerm(ull term){
        terms.push_back(term);
    }
    void setCheck(){
        check = true;
    }
    bool getCheck(){
        return check;
    }
    void setEPI(){
        isEPI = true;
    }
    bool getEPI(){
        return isEPI;
    }
    void printTemrs(){
        for(int i = 0; i < terms.size(); ++i)
            cout << terms[i] << ", ";
        cout << endl;
    }
    Impcnt returnCombined(const Impcnt& another){
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
    ImcntBits getBits(){
        return bits;
    }
    friend Impcnt;
};
class Tabular{
private:
    vector<ull> minterms, dontcares;
    vector<Impcnt> PIs;
    int numberOfInputs;
    void addMin(ull m){
        minterms.push_back(m);
        numberOfInputs = max(numberOfInputs, (int)log2(m) + 1);
    }
    void addDon(ull d){
        dontcares.push_back(d);
        numberOfInputs = max(numberOfInputs, (int)log2(d) + 1);
    }
    int countBit(ull b){
        int cnt = 0;
        for(; b; b >>= 1) cnt += (b & 1);
        return cnt;
    }
public:
    Tabular(){
        numberOfInputs = 0;
    }
    Tabular(vector<ull>& mins, vector<ull>& donts){
        numberOfInputs = 0;
        for(auto m: mins)
            addMin(m);
        for(auto d: donts)
            addDon(d);
    }
    void addMinterm(ull m){
        addMin(m);
    }
    void addDontcare(ull d){
        addDon(d);
    }
    void getPI(){
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
                        cout << lists[i][j][k].getBits().toString() << endl;
                    }
        for(int i = 0; i < PIs.size(); ++i){
            PIs[i].printTemrs();
        }
    }
};
int main(){
    vector<ull> minterms = {0, 2, 5, 6, 7, 8, 9, 13};
    vector<ull> dontcares = {1, 12, 15};
    Tabular tabular(minterms, dontcares);
    tabular.getPI();
    return 0;
}