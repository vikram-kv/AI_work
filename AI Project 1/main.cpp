#include <bits/stdc++.h>
using namespace std;
#define real long double
#define CMAX 120
#define coords pair<real, real>
#define graph vector<vector<real>>
#define INF 1e12

// think about dp code for approx 25 cities
struct GeneticAlgorithm {
    int citycount;
    graph costmatrix;
    vector<pair<vector<int>, real>> population;
    vector<vector<int>> matingPool;
    vector<vector<int>> elitePool;
    vector<vector<int>> childPopulation;
    vector<int> bestTour;
    real bestCost;
    real random01() {
        return ((real)rand())/RAND_MAX;
    }
    int randmod(int x) {
        return rand() % x;
    }
    real evaluateCost(vector<int>& tour, bool bestFlag) {
        real pathcost = 0;
        for(int i=0;i<citycount;++i) {
            int cur = tour[i], next;
            if (i+1 < citycount) 
                next = tour[i+1];
            else 
                next = tour[0];
            pathcost += costmatrix[cur][next];
        }
        if (bestFlag && (pathcost < bestCost)) {
            bestTour = tour;
            bestCost = pathcost;
        }
        return pathcost;
    }
public:
    GeneticAlgorithm(int cnt, graph& costmatrix) {
        this->citycount = cnt;
        this->costmatrix = costmatrix;
        this->bestCost = 1e12;
    }

    vector<vector<int>> ordercrossover(vector<int>& par1, vector<int>& par2) {

        int r1 = randmod(citycount), r2 = randmod(citycount);
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


        vector<vector<int>> children(2);
        children[0] = child1;
        children[1] = child2;
        return children;
    }

    void createInitialPopulation(int popSize) {
        population.clear();

        population = vector<pair<vector<int>,real>>(popSize);
        vector<int> cityids;

        for(int i=0;i<citycount;++i) 
            cityids.push_back(i);

        bestCost = INF;
        // add greedy tour to population
        for(auto& p : population) {
            random_shuffle(cityids.begin(), cityids.end());
            p.first = cityids;
            p.second = evaluateCost(p.first,true);
        }
    }

    void createMatingPool(int eliteSize) {
        matingPool.clear();
        elitePool.clear();

        auto fitnessKey = [] (const pair<vector<int>, real> l, const pair<vector<int>, real>& r) {
            return l.second < r.second;
        };
        sort(population.begin(), population.end(), fitnessKey);

        for(int i=0;i<eliteSize;++i) {
            elitePool.push_back(population[i].first);
        }
        
        real totalFitness = 0;
        for(auto& p : population) {
            totalFitness += 1/p.second;
        }

        vector<real> cumProb(population.size(), 0);
        cumProb[0] = (1/population[0].second)/totalFitness;
        for(int i=1;i<population.size();++i) {
            cumProb[i] = cumProb[i-1] + (1/population[i].second)/totalFitness;
        }

        for(auto& x : cumProb) {
            x *= 100.0;
        }

        int popSize = population.size();
        for(int i=eliteSize;i<popSize;++i) {
            real u = random01() * 100.0;
            for(int j=0;j<popSize;++j) {
                if (cumProb[j] > u) {
                    matingPool.push_back(population[j].first);
                    break;
                }
            }
        }
        assert(matingPool.size() == popSize - eliteSize);
    }

    void createChildPopulation() {
        childPopulation.clear();
        childPopulation = vector<vector<int>> (elitePool.begin(), elitePool.end());
        random_shuffle(matingPool.begin(), matingPool.end());
        for(int i=0;i<matingPool.size();i+=2) {
            auto x = ordercrossover(matingPool[i], matingPool[i+1]);
            childPopulation.push_back(x[0]);
            childPopulation.push_back(x[1]);
        }
        assert(childPopulation.size() == population.size());
    }

    // try limiting max mutations in a generation
    void introduceMutations(real mutRate) {
        real r;
        for(auto& c : childPopulation) {
            r = random01();
            if (r < mutRate/100.0) {
                vector<int> tcopy = c;
                int x = randmod(citycount);
                int y = randmod(citycount);

                int l = min(x,y);
                int r = max(x,y);

                for(int i=l;i<r;++i) {
                    tcopy[i] = c[l+r-1-i];
                }
                c = tcopy;
            }
        }

        // if saturation is detected, random shuffle ->
    }

    // new fitness functions
    // mating pool changes implementation
    void runner(int popSize, int eliteSize, real mutRate, int epochs) {
        createInitialPopulation(popSize);
        int counter = 0;
        while(counter < epochs) {
            createMatingPool(eliteSize);
            createChildPopulation();
            introduceMutations(mutRate);

            for(int i=0;i<popSize;++i) {
                population[i].first = childPopulation[i];
                population[i].second = evaluateCost(population[i].first, true);
            }
            for(int i=0;i<citycount;++i) {
                cout << bestTour[i] << " \n"[i==citycount-1];
            }
            cout << "cost = " << bestCost << '\n';
            ++counter;
            cout << counter << '\n';
        }
    }
};

int main() {
    /**
    srand(std::time(0));
    int citycount;
    char type[50];
    cin.getline(type, 50, '\n');
    cin >> citycount;

    if (citycount > CMAX) {
        cout << "Too many cities\n"; return -1;
    }
    
    vector<coords> cities(citycount);
    for(int i=0;i<citycount;++i) {
        real x, y;
        cin >> x >> y;
        cities[i].first = x; cities[i].second = y;
    }

    vector<vector<real>> costmatrix(citycount);
    for(int i=0;i<citycount;++i) {
        for(int j=0;j<citycount;++j) {
            real x;
            cin >> x;
            costmatrix[i].push_back(x);
        }
    }
    **/
    srand(time(NULL));
    int citycount;
    cout << setprecision(12);
    cin >> citycount;
    graph costmatrix(citycount);
    for(int i=0;i<citycount;++i) {
        for(int j=0;j<citycount;++j) {
            real x;
            cin >> x;
            costmatrix[i].push_back(x);
        }
    }
    GeneticAlgorithm ga(citycount, costmatrix);
    // eliteSize = even, n = even, matingSize = even
    ga.runner(1000,50,2,2000);
    // 1159 best for sample_50
    return 0;
}
