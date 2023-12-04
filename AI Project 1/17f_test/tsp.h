#ifndef TSPH
#define TSPH
#include <bits/stdc++.h>
using namespace std;
#define real long double
#define CMAX 500
#define coords pair<real, real>
#define graph vector<vector<real>>
#include <unistd.h>
real fitness(graph& costmatrix, int citycount, vector<int>& tour);
void fixcity(vector<int>& tour);
bool checkvalidity(vector<int>& tour, int ccount);
real random01();

// union find data structure for greedy heuristic. Implemented in greedy.cpp
class UnionFind {
    int sz;
    vector<int> parents;
public:
    UnionFind();
    UnionFind(int nsz);
    int find(int i);
    void merge(int i, int j);
};

// class for greedy heuristic. Implemented in greedy.cpp
class GreedyHeuristic {
    int citycount;
    graph costmatrix;
    vector<int> greedyTour;
public:
    GreedyHeuristic();
    GreedyHeuristic(graph& costs, int ccount);
    void runner();
    vector<int> getGreedyTour();
};

class KOpt {
    int citycount;
    graph costmatrix;
    real epsilon;
public:
    KOpt();
    KOpt(graph& costs, int ccount);
    vector<int> two_opt(vector<int>& tour);
    void two_optify(vector<int>& tour, int i, int j);
    vector<int> three_opt(vector<int>& tour);
    bool three_optify(vector<int>& v, int i, int j, int k);
};

class SimulatedAnnealing {
    int citycount;
    graph costmatrix;
    real tinitial;
    real tfinal;
    real alpha;
    int chainlength;
    random_device rd;
    default_random_engine gen;
    
public:
    SimulatedAnnealing();
    SimulatedAnnealing(graph& costs, int ccount);
    void initSimulatedAnnealing(vector<int>& tour);
    real eval(real diff, real T);
    void cooldown(real& T);
    vector<int> genNeighbour(vector<int>& tour, int a, int b);
    vector<int> runner(vector<int>& tour);
};

// handle saturation here?
class GeneticAlgorithm {
    int citycount;
    graph costmatrix;
    KOpt KO;
    vector<int> bestTour;
    real bestCost;

    // random device for shuffling, rv distribution generation
    random_device rd;
    default_random_engine gen;

    int popSize;
    int mutDenom;                        // mutRate = 1 / mutDenom
    int retentionSize;
    vector<vector<int>> population;
    vector<real> sfitness;
    bool saturation;
    real epsilon;
    
public:
    GeneticAlgorithm();
    GeneticAlgorithm(graph& costs, int citycount,vector<int>& prevBest);
    void print();
    void updateBest(vector<int>& ntour);
    void initGeneticAlgorithm();
    vector<vector<int>> selectOnFitness();
    void scaleFitness();
    pair<vector<int>, vector<int>> ordercrossover(vector<int>& par1, vector<int>& par2);
    void introduceMutations(vector<vector<int>>&  childPopulation);
    void restoreSize();
    void oneCycle();
    void correctSaturation();
    void runner(long long runs);
};

class TSP {
    int citycount;
    graph costmatrix;
    vector<int> bestTour;
    real bestCost;
    SimulatedAnnealing SA;
    GreedyHeuristic GH;
    KOpt KO;

public:
    TSP(graph costs, int ccount) : SA{costs, ccount} {
        costmatrix = costs;
        citycount = ccount;

        bestTour = vector<int> (ccount);
        iota(bestTour.begin(), bestTour.end(), 0);
        random_shuffle(bestTour.begin(), bestTour.end());
        bestCost = fitness(costmatrix, citycount, bestTour);
        print();

        GH = GreedyHeuristic{costmatrix, citycount};
        KO = KOpt{costmatrix, citycount};
    }
    void print();
    void updateBest(vector<int>& ntour);
    void solve();
};
#endif