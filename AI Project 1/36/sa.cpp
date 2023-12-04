#include "TSP.h"

double TSP::randomfrac() {
    std::random_device rd;
    std::default_random_engine eng(rd());
    std::uniform_real_distribution<double> distr(0, 1);

    return distr(eng);
}

double TSP::eval(double diff, double T) {
    return exp(-diff/T);
}

void TSP::cooldown(double& T) {
    T *= alpha;
}

void TSP::samanager(int runs) {
    vector<int> bestsa = solution;
    // for(int i = 0; i < n; i++) bestsa.push_back(i);
    double saopt = INFINITY;
    int count = 0;
    bool inf = runs < 0;
    sainit();
    while (inf || count++ < runs) {
        vector<int> v = bestsa;
        // shuffle(v.begin()+1, v.end(), gen);
        v = saiter(v);
        double t = fitness(v);
        if(t < saopt) {
            saopt = t;
            bestsa = v;
#ifdef DEBUGSA
            cerr << t << endl;
#endif
        }
        if(t < globalbest) {
            globalbest = t;
            solution = v;
            print();
            // koptmanager(solution);
        }
    }
    
}

void TSP::sainit() {
#ifdef DEBUG
    cerr << "Init sa...";
#endif
    tinitial = INFINITY;
    tfinal = 1.0e-20;
    alpha = 0.99;
    inneriterations = 100;
#ifdef DEBUG
    cerr << "done.\n";
#endif
}

vector<int> TSP::saiter(vector<int> v) {
    vector<int> node = v;

    vector<int> bestnode = node;
    double currbest = fitness(node);

    double T = tinitial;
    // long long stime = curtime();

    // long long time = 1;
    while(T > tfinal) {
        int itercount = 1;
        // cerr << "temperature now is " << T << endl;
        while(itercount <= inneriterations) {
            uniform_int_distribution<> dist(1, n-2);
            vector<int> neighbour = opt2helper(node, dist(gen), dist(gen));
            double fit1 = fitness(neighbour);
            double diff = fit1 - fitness(node);
            if(diff < 0 || randomfrac() < eval(diff, T)) {
                node = neighbour;
                if(fit1 < currbest) {
                    currbest = fit1;
                    bestnode = neighbour;
                }
            }
            itercount++;
        }
        cooldown(T);
    }
    // for(int i = 0; i < n; i++) {
    //     cerr << bestnode[i] << " ";
    // }
    // cerr << endl << currbest << endl;
    return bestnode;
}