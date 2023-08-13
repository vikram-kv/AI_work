#include "TSP.h"

void TSP::primitive() {
    solution = christofides();
    regulate(solution);
    double val = fitness(solution);
    globalbest = val;
    print();
    if(symmetric)
    koptmanager(solution);
    // if(n >= 300) {
    //     vector<int> t = opt3(solution);
    // if(fitness(t) < globalbest) {
    //     globalbest = fitness(t);
    //     solution = t;
    //     print();
    // }
    // }
    
}

TSP::TSP() {
    int n = 10;
    solution = vector<int>(n);
    for(int i = 0; i < n; i++) solution[i] = i;
    gen = default_random_engine(rd());
    globalbest = INFINITY;
}

TSP::TSP(int _n) {
    int n = _n;
    solution = vector<int>(n);
    for(int i = 0; i < n; i++) solution[i] = i;
    print();
}

void TSP::parse() {
    // string input;
    // cin >> input;
    // if(input == "euclidean") {
    //     euclidean = true;
    //     symmetric = true;
    // } else {
    //     cin >> input;
    //     euclidean = false;
    // }
    euclidean = false;
    cin >> n;
    // int waste;
    // coords = vector<pair<double, double>> (n);
    // for(int i = 0; i < n; i++) {
    //     cin >> coords[i].first >> coords[i].second;
    // }
    mat = vector<vector<double>> (n);
    symmetric = true;
    for(int i = 0; i < n; i++) {
        mat[i] = vector<double> (n);
        for(int j = 0; j < n; j++) {
            cin >> mat[i][j];
            if(j <= i) {
                if(mat[i][j] != mat[j][i]) symmetric = false;
            }
        }
    }
    if(euclidean) symmetric = true;
    if(euclidean) update();
    // basic 'random' solution
    solution = vector<int>(n);
    for(int i = 0; i < n; i++) solution[i] = i;
    print();
}

void TSP::update() {
    for(int i = 0; i < n; i++) {
        mat[i][i] = 0;
    }
    for(int i = 0; i < n; i++) {
        for(int j = i + 1; j < n; j++) {
            double t = distance(coords[i], coords[j]);
            mat[i][j] = t;
            mat[j][i] = t;
        }
    }
}

double TSP::distance(pair<double, double> p1, pair<double, double> p2) {
    return sqrt((p1.first-p2.first)*(p1.first-p2.first) + (p1.second-p2.second)*(p1.second-p2.second));
}

void TSP::print() {
    // cout << "\r";
    // for (auto i : solution)
    // {
    //     cout << i << " "; 
    // }
    // cout << endl;
    cout << fitness(solution) << endl;
#ifdef DEBUG
    cerr << fitness(solution) << endl;
#endif
}

void TSP::regulate(vector<int>& v) {
    for(int i = 0; i < n; i++) {
        if(*v.begin() == 0) break;
        int t = *v.begin();
        v.erase(v.begin());
        v.push_back(t);
    }
}