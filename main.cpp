/*

title: Tabular method solver
author: 윤상건 (Sang-geon Yun, ggj06281@kookmin.ac.kr)

*/

#include <iostream>
#include <vector>
#include "headers/TabularMethodSolver.h"
using namespace std;

typedef unsigned long long int ull;

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
        string trueSolution = tabular.solve(false, true);
        string approxSolution = tabular.solve(true);
        cout << "True solution: " << trueSolution << endl;
        cout << "Approximation: " << approxSolution << endl;
        cout << endl;
    }
    
    return 0;
}