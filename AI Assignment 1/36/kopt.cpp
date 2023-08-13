#include "TSP.h"

void TSP::koptmanager(vector<int> in) {
    vector<int> t;
    // if(n < 1000) {
        t = opt2(in);
        if(fitness(t) < globalbest) {
            globalbest = fitness(t);
            solution = t;
            print();
        }
    // }
    // if(n < 300) {
        t = opt3(t);
        if(fitness(t) < globalbest) {
            globalbest = fitness(t);
            solution = t;
            print();
        }
    // }   
}

vector<int> TSP::opt2(vector<int> in) {
    vector<int> out = in;
    double start = fitness(in);
    pair<double,vector<int>> t;
    while(true) {
        t = opt2iter(out);
        if(t.first >= start) break;
        out = t.second;
        start = t.first;
        if(start < globalbest) {
            solution = t.second;
            print();
            globalbest = start;
        }
    }
    regulate(out);
    return out;
}

pair<double,vector<int>> TSP::opt2iter(vector<int> curr) {
    vector<int> best = curr;
    double lbest = fitness(curr);
    double fit = lbest;
    vector<int> t;
    for(int i = 1; i < n-2; i++) {
        for(int j = i+2; j < n; j++) {
            t = opt2helper(curr, i, j);
            double f = fit - mat[curr[i-1]][curr[i]] - mat[curr[j-1]][curr[j]] + mat[curr[i-1]][curr[j-1]] + mat[curr[i]][curr[j]];
            if(f < lbest) {
                lbest = f;
                best = t;
            }
        }
    }
    return make_pair(lbest, best);
}

vector<int> TSP::opt3(vector<int> in) {
    vector<int> out = in;
    double start = fitness(in);
    vector<int> t;
    while(true) {
        t = opt3iter(out);
        if(fitness(t) >= start) break;
        out = t;
        start = fitness(t);
        if(start < globalbest) {
            solution = t;
            print();
            globalbest = start;
        }
    }
    regulate(out);
    return out;
}

vector<int> TSP::opt2helper(vector<int> v, int i, int j) {
    vector<int> ret(v);
    assert(i >= 0 && i < n);
    assert(j >= 0 && j < n);
    if(i <= j) {
        reverse(ret.begin()+i, ret.begin()+j);
    } else {
        reverse(ret.begin()+j, ret.begin()+i);
    }
    return ret;
}

vector<int> TSP::opt3iter(vector<int> curr) {
    vector<int> best = curr;
    double lbest = fitness(curr);
    vector<int> t;
    for(int i = 1; i < n-4; i++) {
        for(int j = i+2; j < n-2; j++) {
            for(int k = j+2; k < n; k++) {
                t = opt3helper(curr, i, j, k);
                double f = fitness(t);
                if(f < lbest) {
                    lbest = f;
                    best = t;
                }
            }
        }
    }
    return best;
}

vector<int> TSP::opt3helper(vector<int> v, int i, int j, int k) {
    vector<int> ret(v);
    assert(i >= 0 && i < n);
    assert(j >= 0 && j < n);
    assert(k >= 0 && k < n);
    double d = INFINITY;
    double f;
    vector<int> t;
    t = v;
    reverse(t.begin()+i, t.begin()+j);
    f = fitness(t);
    if(f < d) {
        d = f;
        ret = t;
    }
    t = v;
    reverse(t.begin()+i, t.begin()+j);
    reverse(t.begin()+j, t.begin()+k);
    f = fitness(t);
    if(f < d) {
        d = f;
        ret = t;
    }
    t = v;
    reverse(t.begin()+j, t.begin()+k);
    f = fitness(t);
    if(f < d) {
        d = f;
        ret = t;
    }

    // swap segments ij and jk
    vector<int> t1;
    for(int x = 0; x < i; x++) {
        t1.push_back(v[x]);
    }
    for(int x = j; x < k; x++) {
        t1.push_back(v[x]);
    }
    for(int x = i; x < j; x++) {
        t1.push_back(v[x]);
    }
    for(int x = k; x < n; x++) {
        t1.push_back(v[x]);
    }
    t = t1;
    f = fitness(t);
    if(f < d) {
        d = f;
        ret = t;
    }

    t = t1;
    reverse(t.begin()+i, t.begin()+j);
    f = fitness(t);
    if(f < d) {
        d = f;
        ret = t;
    }
    // t = t1;
    // reverse(t.begin()+i, t.begin()+j);
    // reverse(t.begin()+j, t.begin()+k);
    // f = fitness(t);
    // if(f < d) {
    //     d = f;
    //     ret = t;
    // }
    t = t1;
    reverse(t.begin()+j, t.begin()+k);
    f = fitness(t);
    if(f < d) {
        d = f;
        ret = t;
    }
    return ret;
}