#include "tsp.h"

void TSP::print() {
    for(auto c : bestTour) {
        cout << c << ' ';
    } cout << endl;
    cout << bestCost << endl;
}

void TSP::updateBest(vector<int>& ntour) {
    if(!checkvalidity(ntour, citycount)) return ;
    real cost = fitness(costmatrix, citycount, ntour);
    if (cost < bestCost) {
        bestCost = cost;
        bestTour = ntour;
        print();
    }
}

void TSP::solve() {
    GH.runner();
    vector<int> greedyTour = GH.getGreedyTour();
    updateBest(greedyTour);

    // boosting greedy tour with 2-opt
    vector<int> gopt2tour = KO.two_opt(greedyTour);
    updateBest(gopt2tour);

    // boosting greedy tour with 3-opt
    vector<int> gopt3tour = KO.three_opt(greedyTour);
    updateBest(gopt3tour);


    // boosting greedy tour with simulated annealing
    int saruns = 6;
    if (citycount <= 100) {
        saruns = 15;
    } else if (citycount <= 200) {
        saruns = 9;
    }

    cout << "SA GREED_BOOST START" << endl;
    // greedy tour with simulated annealing followed by k-opt
    vector<int> gsatour, gsaopt2tour, gsaopt3tour;
    for(int i=0;i<saruns;++i) {
        gsatour = SA.runner(greedyTour);
        updateBest(gsatour);
        gsaopt2tour = KO.two_opt(gsatour);
        updateBest(gsaopt2tour);
        vector<int> gsaopt3tour = KO.three_opt(gsatour);
        updateBest(gsaopt3tour);
    }

    cout << "SA RAND START\n";
    // simulated annealing, 2-opt and 3-opt for some random tours
    vector<int> test(citycount,0); 
    iota(test.begin(), test.end(), 0);
    vector<int> saver;
    for(int i=0;i<saruns;++i) {
        random_shuffle(test.begin(), test.end());
        saver = SA.runner(test);
        updateBest(saver);
        saver = KO.two_opt(test);
        updateBest(saver);
        saver = KO.three_opt(test);
        updateBest(saver);
    }

    // genetic algorithm runs - final thing :)
    cout << "GEN START\n";
    GeneticAlgorithm GA(costmatrix, citycount, bestTour);
    GA.runner(-1);
}