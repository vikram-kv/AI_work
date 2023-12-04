#ifndef TSP_H
#define TSP_H
#define DEBUG
#define DEBUGSA
#include <iostream>
#include <iomanip>
#include <bits/stdc++.h>
using namespace std;

enum GENMESSAGE {
    NIL,
    SATURATION
};

class TSP {
    public:
    vector<int> solution;
    int n;
    vector<pair<double,double>> coords;
    vector<vector<double>> mat;
    bool euclidean;
    bool symmetric;
    double globalbest;

    //ds for genetic
    vector<vector<int>> population;
    vector<double> scores;
    int psize;
    int mratio;
    int rsize;
    GENMESSAGE message;

    // randomizer
    random_device rd;
    default_random_engine gen;

    // methods
    TSP(); 
    TSP(int);
    void parse(); //done
    void update(); //done
    double distance(pair<double, double>, pair<double, double>); //done
    void print(); //done
    void primitive(); //done
    void regulate(vector<int>&);

    // genetic
    void managegenetic(int);
    void initgenetic();
    void repairgenetic();
    void geneticiter();
    void evaluate();
    vector<vector<int>> select();
    void crossover(vector<int>, vector<int>, vector<int>&, vector<int>&);
    void retain();
    void mutate(vector<int>&);
    void alienate();

    // genetic helpers
    double fitness(vector<int>); //done
    void pmxcross(vector<int>, vector<int>, vector<int>&, vector<int>&);
    int cyclecross(vector<int>, vector<int>, vector<int>&, vector<int>&);
    void ordinalcross(vector<int>, vector<int>, vector<int>&, vector<int>&); //done
    vector<int> p2o(vector<int>); //done
    vector<int> o2p(vector<int>); //done

    // k opt
    void koptmanager(vector<int>);
    vector<int> opt2(vector<int>);
    pair<double,vector<int>> opt2iter(vector<int>);
    vector<int> opt2helper(vector<int> v, int i, int j);
    void opt2random();
    vector<int> opt3(vector<int>);
    vector<int> opt3iter(vector<int>);
    vector<int> opt3helper(vector<int>, int, int, int);
    void opt3random();

    //for euclidean approximations
    vector<int> odds;
    vector<vector<int>> adjlist; // for mst
    vector<vector<int>> ocosts; // for christofides
    vector<int> euler; 
    void eulertour(int, vector<int>&);
    void makehamilton(vector<int>& path, float& cost);
    vector<int> christofides();

    // simulated annealing
    double tinitial;
    double tfinal;
    double alpha;
    int inneriterations;

    void samanager(int);
    void sainit();
    vector<int> saiter(vector<int>);

    //sa helpers
    double randomfrac();
    void cooldown(double&);
    double eval(double, double);
};


#endif