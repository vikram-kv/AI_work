#include <iostream>
#include <fstream>
#include "TSP.h"
#include <time.h>
#include <unistd.h>
#include <bits/stdc++.h>
using namespace std;

bool euclidean;

int main() {
    time_t* t = new time_t;
    long origin = time(t);

    cout << fixed << setprecision(9);
    cerr << fixed << setprecision(9);
    // TSPCalc tspcalc;

    // string input;
    // cin >> input;
    // if(input == "euclidean") {
    //     tspcalc.euclidean = true;
    // } else {
    //     cin >> input;
    //     tspcalc.euclidean = false;
    // }
    // tspcalc.parseinput();
    // if(tspcalc.n <= 10) {
    //     tspcalc.bruteforce();
    //     exit(0);
    // }
    // tspcalc.primitive();
    // int count = 0;

    // tspcalc.initgenetic();
    // while (true)
    // {
    //     count = 0;
    //     while(count++ < 30000) tspcalc.solve3();
    //     count = 0;
    //     while (count++ < 100) tspcalc.solve();
    //     tspcalc.repairgenetic();
    // }
    TSP tsp;
    tsp.parse();
    tsp.primitive();
    /* set -1 for INFINITE ITERATIONS */
    if(tsp.n < 1000)
    tsp.managegenetic(-1);
    else
    tsp.samanager(-1);
    
}
