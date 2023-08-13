#include <bits/stdc++.h>
#include "tsp.h"
using namespace std;

int main() {
    /*
    int citycount;
    char type[50];
    cin.getline(type, 50, '\n');
    cin >> citycount;
    
    
    vector<coords> cities(citycount);
    for(int i=0;i<citycount;++i) {
        real x, y;
        cin >> x >> y;
        cities[i].first = x; cities[i].second = y;
    }

    graph costmatrix(citycount);
    for(int i=0;i<citycount;++i) {
        for(int j=0;j<citycount;++j) {
            real x;
            cin >> x;
            costmatrix[i].push_back(x);
        }
    }
---------
    int citycount;
    cin >> citycount;
    graph costmatrix(citycount);
    for(int i=0;i<citycount;++i) {
        for(int j=0;j<citycount;++j) {
            real x;
            cin >> x;
            costmatrix[i].push_back(x);
        }
    }
    */
    srand(time(NULL));
    int citycount;
    char type[50];
    cin.getline(type, 50, '\n');
    cin >> citycount;
    
    vector<coords> cities(citycount);
    for(int i=0;i<citycount;++i) {
        real x, y;
        cin >> x >> y;
        cities[i].first = x; cities[i].second = y;
    }

    graph costmatrix(citycount);
    for(int i=0;i<citycount;++i) {
        for(int j=0;j<citycount;++j) {
            real x;
            cin >> x;
            costmatrix[i].push_back(x);
        }
    }
    TSP tsp(costmatrix, citycount);
    tsp.solve();
    return 0;
}