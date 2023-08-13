#include "tsp.h"

GeneticAlgorithm::GeneticAlgorithm() {
    costmatrix = {};
    citycount = 0;
    gen = default_random_engine(rd());
    KO = KOpt{};
    epsilon = 0.01;
}

GeneticAlgorithm::GeneticAlgorithm(graph& costs, int ccount, vector<int>& prevBest) {
    costmatrix = costs;
    citycount = ccount;
    gen = default_random_engine(rd());
    KO = KOpt{costmatrix, ccount};
    bestTour = prevBest;
    bestCost = fitness(costmatrix, citycount, bestTour);
    epsilon = 0.01;
}

void GeneticAlgorithm::print() {
    for(auto c : bestTour) {
        cout << c << ' ';
    } cout << endl;
    cout << "GEN - " << bestCost << endl;
}

void GeneticAlgorithm::updateBest(vector<int>& ntour) {
    if(!checkvalidity(ntour, citycount)) return ;
    real cost = fitness(costmatrix, citycount, ntour);
    if (cost < bestCost) {
        bestCost = cost;
        bestTour = ntour;
        print();
    }
}

void GeneticAlgorithm::initGeneticAlgorithm() {
    popSize = min(8 * citycount, 1200);
    mutDenom = 25;            // 4% mutation
    retentionSize = min(4 * citycount, 600);
    population = vector<vector<int>>(popSize);
    sfitness = vector<real>(popSize);
    vector<int> temp = vector<int>(citycount);
    iota(temp.begin(), temp.end(), 0);
    for(int i=0;i<popSize;++i) {
        shuffle(temp.begin(), temp.end(), gen);
        population[i] = temp;
    }
    saturation = false;
}

void GeneticAlgorithm::scaleFitness() {
    real maxFitness = fitness(costmatrix, citycount, population.back());
    for(int i=0;i<popSize;++i) {
        sfitness[i] = maxFitness/fitness(costmatrix, citycount, population[i]);
    }
    if (sfitness[0] <= 1.0 + epsilon) saturation = true;
}

vector<vector<int>> GeneticAlgorithm::selectOnFitness() {
    vector<vector<int>> retentionPool;
    for(int i = 0; i < popSize; i++) {
        int rnd = round(sfitness[i]);
        while(rnd > 0) {
            retentionPool.push_back(population[i]);
            --rnd;
        }
    }

    if (retentionPool.size() > retentionSize) {
        retentionPool.resize(retentionSize);
    }
    // snip to handle small pool in case it happens
    while(retentionPool.size() < retentionSize) {
        vector<int> tour = bestTour;
        random_shuffle(tour.begin(), tour.end());
        retentionPool.push_back(tour);
    }
    shuffle(retentionPool.begin(), retentionPool.end(), gen);
    return retentionPool;
}


pair<vector<int>, vector<int>> GeneticAlgorithm::ordercrossover(vector<int>& par1, vector<int>& par2) {

    uniform_int_distribution<> dist(0,citycount-1);
    int r1 = dist(gen), r2 = dist(gen);
    int start = min(r1, r2), end = max(r1, r2);
    vector<int> child1(citycount, -1), child2(citycount, -1);

    set<int> inc1, inc2;
    for(int i=start; i<=end; ++i) {
        child1[i] = par1[i];
        inc1.insert(par1[i]);
        child2[i] = par2[i];
        inc2.insert(par2[i]);
    }

    int p1ptr = 0, p2ptr = 0;
    for(int i=0; i<citycount; ++i) {
        
        if (child1[i] == -1) {
            while (inc1.find(par2[p2ptr]) != inc1.end()) ++p2ptr;
            child1[i] = par2[p2ptr]; ++p2ptr;
        }

        if (child2[i] == -1) {
            while (inc2.find(par1[p1ptr]) != inc2.end()) ++p1ptr;
            child2[i] = par1[p1ptr]; ++p1ptr;
        }
    }

    return {child1, child2};
}

// try limiting max mutations in a generation
void GeneticAlgorithm::introduceMutations(vector<vector<int>>& childPopulation) {
    int r;
    uniform_int_distribution<> dist(0,citycount-1);
    uniform_int_distribution<> mdist(1,mutDenom);
    for(auto& c : childPopulation) {
        r = mdist(gen);
        if (r == 1) {
            vector<int> tcopy = c;
            int x = dist(gen), y = dist(gen);
            int l = min(x,y), r = max(x,y);
            for(int i=l;i<=r;++i) {
                tcopy[i] = c[l+r-i];
            }
            c = tcopy;
        }
    }
    // if saturation is detected, random shuffle ?
}

void GeneticAlgorithm::restoreSize() {
    auto sorter = [&] (vector<int> l) {return fitness(costmatrix,citycount, l);}; 
    sort(population.begin(), population.end(), [&] (const vector<int> &l, const vector<int> &r) {return sorter(l) < sorter(r);});
    population.resize(popSize);
}

void GeneticAlgorithm::oneCycle() {
    auto sorter = [&] (vector<int> l) {return fitness(costmatrix,citycount, l);}; 
    sort(population.begin(), population.end(), [&] (const vector<int> &l, const vector<int> &r) {return sorter(l) < sorter(r);});
    scaleFitness();
    vector<vector<int>> matingPool = selectOnFitness();

    vector<vector<int>> childPopulation;
    for(int i=0;i<retentionSize;i+=2) {
        vector<int> par1 = matingPool[i];
        vector<int> par2 = matingPool[i+1];

        pair<vector<int>, vector<int>> children = ordercrossover(par1, par2);
        childPopulation.push_back(children.first);
        childPopulation.push_back(children.second);
    }
    introduceMutations(childPopulation);
    for(auto& c : childPopulation) {
        population.push_back(c);
    }
    restoreSize();
}

void GeneticAlgorithm::runner(long long runs=-1) {

    bool loopforever = (runs == -1);
    initGeneticAlgorithm();
    long long cntr = 1;
    vector<int> genBest;

    while(loopforever || (cntr <= runs)) {
        oneCycle();
        genBest = population[0];
        updateBest(genBest);

        //3-opt on genbest
        genBest = population[0];
        genBest = KO.three_opt(genBest);
        updateBest(genBest);

        //2-opt on genbest
        genBest = population[0];
        genBest = KO.two_opt(genBest);
        updateBest(genBest);

        if (saturation) cout << "SAT\n";
        if (saturation) initGeneticAlgorithm();
        ++cntr;
    }
}