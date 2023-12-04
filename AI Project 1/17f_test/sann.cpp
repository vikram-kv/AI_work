#include "tsp.h"

SimulatedAnnealing::SimulatedAnnealing() {
    costmatrix = {};
    citycount = 0;
    gen = default_random_engine(rd());
}

SimulatedAnnealing::SimulatedAnnealing(graph& costs, int ccount) {
    costmatrix = costs;
    citycount = ccount;
    gen = default_random_engine(rd());
}

void SimulatedAnnealing::initSimulatedAnnealing(vector<int>& tour) {
    tinitial = 10000*fitness(costmatrix, citycount, tour);
    tfinal = 1e-10;
    alpha = 0.995;
    chainlength = 100;
}

real SimulatedAnnealing::eval(real diff, real T) {
    return exp(-diff/T);
}

void SimulatedAnnealing::cooldown(real& T) {
    T *= alpha;
}

vector<int> SimulatedAnnealing::genNeighbour(vector<int>& tour, int a, int b) {
        vector<int> tcopy = tour;
        int l = min(a,b);
        int r = max(a,b);

        for(int i=l;i<=r;++i) {
            tcopy[i] = tour[l+r-i];
        }
        return tcopy;
}

vector<int> SimulatedAnnealing::runner(vector<int>& tour) {
    vector<int> curTour = tour;
    vector<int> bestTour = tour;
    real bestCost = fitness(costmatrix, citycount, curTour);
    initSimulatedAnnealing(tour);
    real T = tinitial;
    while(T > tfinal) {
        int counter = 1;
        while(counter <= chainlength) {
            int n = citycount-1;
            uniform_int_distribution<> dist(1, citycount-1);
            vector<int> next = genNeighbour(curTour, dist(gen), dist(gen));
            double fitn = fitness(costmatrix, citycount, next);
            double diff = fitn - fitness(costmatrix, citycount, curTour);
            if(diff < 0 || random01() < eval(diff, T)) {
                curTour = next;
                if(fitn < bestCost) {
                    bestCost = fitn;
                    bestTour = next;
                }
            }
            counter++;
        }
        cooldown(T);
    }
    return bestTour;
}