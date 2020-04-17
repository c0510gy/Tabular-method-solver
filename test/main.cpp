/*

title: Tabular method solver
author: 윤상건 (Sang-geon Yun, ggj06281@kookmin.ac.kr)

*/

#include <iostream>
#include <vector>
#include <set>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include "headers/TabularMethodSolver.h"
using namespace std;

typedef unsigned long long int ull;
typedef long long int lli;
typedef long double ldb;

void cal(vector<ull>& minterms, vector<ull>& dontcares){
    cout << "Minterms: ";
    for(int i = 0; i < minterms.size(); ++i){
        cout << minterms[i] << ", ";
    }
    cout << endl << "Dont cares: ";
    for(int i = 0; i < dontcares.size(); ++i){
        cout << dontcares[i] << ", ";
    }
    cout << endl;
    Tabular tabular(minterms, dontcares);
    chrono::steady_clock::time_point approxBegin = chrono::steady_clock::now();
    cout << "Approximation: " << tabular.solve(true) << endl;
    chrono::steady_clock::time_point approxEnd = chrono::steady_clock::now();
    cout << "Time difference = " << chrono::duration_cast<chrono::microseconds>(approxEnd - approxBegin).count() << "[µs]" << endl;
    cout << "Time difference = " << chrono::duration_cast<chrono::nanoseconds> (approxEnd - approxBegin).count() << "[ns]" << endl;

    chrono::steady_clock::time_point trueBegin = chrono::steady_clock::now();
    cout << "True solution: " << tabular.solve(false) << endl;
    chrono::steady_clock::time_point trueEnd = chrono::steady_clock::now();
    cout << "Time difference = " << chrono::duration_cast<chrono::microseconds>(trueEnd - trueBegin).count() << "[µs]" << endl;
    cout << "Time difference = " << chrono::duration_cast<chrono::nanoseconds> (trueEnd - trueBegin).count() << "[ns]" << endl;

    cout << endl;
}

pair<lli, int> cal_true(vector<ull>& minterms, vector<ull>& dontcares){
    Tabular tabular(minterms, dontcares);
    chrono::steady_clock::time_point trueBegin = chrono::steady_clock::now();
    int cost = tabular.solveTest(false);
    chrono::steady_clock::time_point trueEnd = chrono::steady_clock::now();
    lli time = chrono::duration_cast<chrono::nanoseconds>(trueEnd - trueBegin).count();
    return pair<lli, int>(time, cost);
}
pair<lli, int> cal_approx(vector<ull>& minterms, vector<ull>& dontcares){
    Tabular tabular(minterms, dontcares);
    chrono::steady_clock::time_point approxBegin = chrono::steady_clock::now();
    int cost = tabular.solveTest(true);
    chrono::steady_clock::time_point approxEnd = chrono::steady_clock::now();
    lli time = chrono::duration_cast<chrono::nanoseconds>(approxEnd - approxBegin).count();
    return pair<lli, int>(time, cost);
}
int randNumber(int s, int e){
    return rand() % (e - s + 1) + s;
}
int main(){
    //srand((unsigned int)time(NULL));
    vector<pair<ldb, ldb>> measureTime, measureCost;

    int sampleSize = 20;
    int inputs = 10;
    for(int mins = 1; mins <= 118; ++mins){
        cout << mins << endl;
        lli totTrueTime = 0, totApproxTime = 0;
        int totTrueCost = 0, totApproxCost = 0;
        for(int t = 0; t < sampleSize; ++t){
            srand((unsigned int)t);
            cout << "Running test #" << t << endl;
            vector<ull> minterms, dontcares;
            set<ull> exi;
            
            int totMins = mins;
            int totDons = 10;
            for(int j = 0; j < totMins; ++j){
                ull n = randNumber(0, (1 << inputs) - 1);
                if(exi.find(n) != exi.end()){
                    --j;
                    continue;
                }
                exi.insert(n);
                minterms.push_back(n);
            }
            for(int j = 0; j < totDons; ++j){
                ull n = randNumber(0, (1 << inputs) - 1);
                if(exi.find(n) != exi.end()){
                    --j;
                    continue;
                }
                exi.insert(n);
                dontcares.push_back(n);
            }

            pair<lli, int> trueInfo = cal_true(minterms, dontcares);
            pair<lli, int> approxInfo = cal_approx(minterms, dontcares);
            totTrueTime += trueInfo.first;
            totApproxTime += approxInfo.first;

            totTrueCost += trueInfo.second;
            totApproxCost += approxInfo.second;
        }
        ldb meanTrueTime = (ldb)totTrueTime / sampleSize;
        ldb meanApproxTime = (ldb)totApproxTime / sampleSize;
        cout << "Time spent: " << endl;
        cout << fixed << "true: " << meanTrueTime << endl << "approx: " << meanApproxTime << endl;
        cout << fixed << "true / approx = " << meanTrueTime / meanApproxTime << endl;
        cout << endl;
        ldb meanTrueCost = (ldb)totTrueCost / sampleSize;
        ldb meanApproxCost = (ldb)totApproxCost / sampleSize;
        cout << "Cost: " << endl;
        cout << fixed << "true: " << meanTrueCost << endl << "approx: " << meanApproxCost << endl;
        cout << fixed << "true / approx = " << meanTrueCost / meanApproxCost << endl;

        measureTime.push_back(pair<ldb, ldb>(meanTrueTime, meanApproxTime));
        measureCost.push_back(pair<ldb, ldb>(meanTrueCost, meanApproxCost));
    }

    for(int j = 0; j < measureTime.size(); ++j)
        cout << fixed << measureTime[j].first << ", " << measureTime[j].second << ", " << measureTime[j].first / measureTime[j].second << endl;
    for(int j = 0; j < measureCost.size(); ++j)
        cout << fixed << measureCost[j].first << ", " << measureCost[j].second << ", " << measureCost[j].first / measureCost[j].second << endl;
    
    freopen("./output_time.txt", "w", stdout);
    for(int j = 0; j < measureTime.size(); ++j)
        cout << fixed << measureTime[j].first << "," << measureTime[j].second << "," << measureTime[j].first / measureTime[j].second << endl;
    freopen("./output_cost.txt", "w", stdout);
    for(int j = 0; j < measureCost.size(); ++j)
        cout << fixed << measureCost[j].first << "," << measureCost[j].second << "," << measureCost[j].first / measureCost[j].second << endl;
    
    //cal(minterms, dontcares);
    
    return 0;
}