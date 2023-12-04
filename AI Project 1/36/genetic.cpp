#include "TSP.h"

void TSP::managegenetic(int runs) {
    initgenetic();
    int count = 0;
    bool inf = runs < 0;
    double geneticbest = INFINITY;
    vector<int> gsol;
    int gsat = 0;
#ifdef DEBUG
    int c = 0;
#endif
    while(inf || count++ < runs) {
        if(gsat > 500) break;
        geneticiter();
#ifdef DEBUG
    cerr << "\rRun : " << ++c;
#endif
        if(message == SATURATION) {
            gsat++;
            message = NIL;
            repairgenetic();
            continue;
        }
        double currbest = fitness(population[0]);
        if(currbest < geneticbest) {
            gsol = population[0];
            geneticbest = currbest;
            gsat = 0;
            if(symmetric)
            koptmanager(gsol);
            // shuffle(population[0].begin()+1, population[0].end(), gen);
            // sort(population.begin(), population.end(), [&](const auto& i, const auto& j) -> bool {return fitness(i) < fitness(j);});
        }
        if(globalbest == INFINITY || geneticbest < globalbest) {
            solution = gsol;
            globalbest = geneticbest;
            print();
            if(symmetric) {
                koptmanager(solution);
                shuffle(population[0].begin()+1, population[0].end(), gen);
                sort(population.begin(), population.end(), [&](const auto& i, const auto& j) -> bool {return fitness(i) < fitness(j);});
            }
        }
    }
}

void TSP::initgenetic() {
    psize = 4*n;
    mratio = 4;
    rsize = 2*n;
    population = vector<vector<int>>(psize);
    scores = vector<double> (psize);
    population[0] = solution;
    for(int i = 1; i < psize; i++) {
        vector<int> addin = vector<int> (n);
        for(int i = 0; i < n; i++) addin[i] = i;
        shuffle(addin.begin()+1, addin.end(), gen);
        population[i] = addin;
        assert(population[i].size() == n);
    }
    sort(population.begin(), population.end(), [&](const auto& i, const auto& j) -> bool {return fitness(i) < fitness(j);});
}

void TSP::geneticiter() {
    evaluate();
    if(message == SATURATION) {retain(); return;}
    message = NIL;
    vector<vector<int>> par = select();
    // vector<vector<int>> children;
    for(int i = 0; i < n; i++) {
        vector<int> p1 = par[2*i];
        vector<int> p2 = par[2*i+1];
        if(p1 == p2) {
            continue;
        }
        vector<int> c1, c2;
        crossover(p1, p2, c1, c2);
        population.push_back(c1);
        population.push_back(c2);
    }
    retain();
}

void TSP::evaluate() {
    double maxval = fitness(population[psize - 1]);
    for(int i = 0; i < psize; i++) {
        scores[i] = maxval / fitness(population[i]);
    }
    assert((*max_element(scores.begin(), scores.end()) == scores[0]));
    if(scores[0] <= 1) message = SATURATION;
}

vector<vector<int>> TSP::select() {
    vector<vector<int>> pool;
    for(int i = 0; i < psize; i++) {
        int close = round(scores[i]);
        while(close-- > 0) {
            pool.push_back(population[i]);
        }
    }
    shuffle(pool.begin(), pool.end(), gen);
    pool.resize(rsize);
    return pool;
}

void TSP::crossover(vector<int> p1, vector<int> p2, vector<int>& c1, vector<int>& c2) {
    // if(cyclecross(p1,p2,c1,c2) < 2)
    ordinalcross(p1, p2, c1, c2);
    // pmxcross(p1, p2, c1, c2);
    uniform_int_distribution<> dist(1, mratio);
    if(dist(gen) == mratio) {
        mutate(c1);
        mutate(c2);
    }
}

void TSP::retain() {
    sort(population.begin(), population.end(), [&](const auto& i, const auto& j) -> bool {return fitness(i) < fitness(j);});
    population.resize(psize);
}


void TSP::repairgenetic() {
    for(int i = 1; i < psize; i++) {
        vector<int> addin = population[i];
        shuffle(addin.begin()+1, addin.end(), gen);
        population[i] = addin;
        assert(population[i].size() == n);
    }
    sort(population.begin(), population.end(), [&](const auto& i, const auto& j) -> bool {return fitness(i) < fitness(j);});
}


void TSP::mutate(vector<int>& c) {
    uniform_int_distribution<> dist(1, n/2);
    int i = dist(gen);
    int j;
    while(j = dist(gen) < i);
    reverse(c.begin()+i, c.end()+j);
}



//helpers


double TSP::fitness(vector<int> sol) {
    if(sol.size() != n) return INFINITY;
    double r = 0;
    for(int i = 0; i < n; i++) {
        if(i == 0) r += mat[sol[n-1]][sol[0]];
        else r += mat[sol[i-1]][sol[i]];
    }
    return r;
}

void TSP::pmxcross(vector<int> p1, vector<int> p2, vector<int>& c1, vector<int>& c2) {
    uniform_int_distribution<> d1(1, n-2);
    int i = d1(gen);
    uniform_int_distribution<> d2(i+1,n-1);
    int j = d2(gen);
    c1 = vector<int> (n, -1);
    c2 = vector<int> (n, -1);
    for(int k = i; k <= j; k++) {
        c1[k] = p1[k];
        c2[k] = p2[k];
    }
    for(int k = i; k <= j; k++) {
        int pick = p2[k];
        if(find(c1.begin(), c1.end(), pick) != c1.end()) continue;
        int currin = k;
        int insi = find(p2.begin(), p2.end(), p1[currin]) - p2.begin();
        while(insi >= i && insi <= j) {
            currin = insi;
            insi = find(p2.begin(), p2.end(), p1[currin]) - p2.begin();
        }
        c1[insi] = pick;
    }
    for(int k = 0; k < n; k++) {
        if(c1[k] == -1) c1[k] = p2[k];
    }
    for(int k = i; k <= j; k++) {
        int pick = p1[k];
        if(find(c2.begin(), c2.end(), pick) != c2.end()) continue;
        int currin = k;
        int insi = find(p1.begin(), p1.end(), p2[currin]) - p1.begin();
        while(insi >= i && insi <= j) {
            currin = insi;
            insi = find(p1.begin(), p1.end(), p2[currin]) - p1.begin();
        }
        c2[insi] = pick;
    }
    for(int k = 0; k < n; k++) {
        if(c2[k] == -1) c2[k] = p1[k];
    }
    regulate(c1);
    regulate(c2);
}

int TSP::cyclecross(vector<int> p1, vector<int> p2, vector<int>& c1, vector<int>& c2) {
    int n = p1.size();
    assert(p2.size()==n);
    // unordered_map<int, int> bucketmap;
    vector<int> cycles(n, 0);
    int topcycle = 0;
    for(int i = 0; i < n; i++) {
        int s1 = p1[i];
        int s2 = p2[i];
        if(cycles[i] == 0) {
            cycles[i] = ++topcycle;
            int start = s2;
            int curri = i;
            int t1 = s1;
            int t2 = s2;
            while(p1[curri] != start) {
                curri = find(p2.begin(), p2.end(), t1) - p2.begin();
                cycles[curri] = cycles[i];
                t1 = p1[curri];
            }
        }
    }
    c1 = vector<int> (n);
    c2 = vector<int> (n);
    for(int i = 0; i < n; i++) {
        int s1 = p1[i];
        int s2 = p2[i];
        if(cycles[i] % 2 == 0) {
            c1[i] = s1;
            c2[i] = s2;
        } else {
            c1[i] = s2;
            c2[i] = s1;
        }
    }
    return topcycle;
}

void TSP::ordinalcross(vector<int> p1, vector<int> p2, vector<int>& c1, vector<int>& c2) {
    vector<int> po1 = p2o(p1);
    vector<int> po2 = p2o(p2);
    vector<int> co1 = vector<int>(n);
    vector<int> co2 = vector<int>(n);
    if(n == 1) {
        c1[1] = p1[1];
        c2[1] = p2[1];
        return;
    }
    uniform_int_distribution<> dist(1, n-2);
    int cross = dist(gen);
    for(int i = 0; i <= cross; i++) {
        co1[i] = po1[i];
        co2[i] = po2[i];
    }
    for(int i = cross+1; i < n; i++) {
        co1[i] = po2[i];
        co2[i] = po1[i];
    }
    c1 = o2p(co1);
    c2 = o2p(co2);
}

vector<int> TSP::o2p(vector<int> o) {
    vector<int> p;
    vector<int> record = vector<int> (o.size());
    for(int i = 0; i < record.size(); i++) record[i] = i;
    for(int i = 0; i < o.size(); i++) {
        int rep = o[i];
        p.push_back(record[rep]);
        record.erase(record.begin()+rep);
    }
    return p;
}

vector<int> TSP::p2o(vector<int> p) {
    vector<int> o;
    vector<int> record = vector<int> (p.size());
    for(int i = 0; i < record.size(); i++) record[i] = i;
    for(int i = 0; i < p.size(); i++) {
        int point = p[i];
        auto k = find(record.begin(), record.end(), point);
        o.push_back(k - record.begin());
        record.erase(k);
    }
    return o;
}