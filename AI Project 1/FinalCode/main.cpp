#include <bits/stdc++.h>
using namespace std;
#define real long double
#define CMAX 120
#define coords pair<real, real>
#define graph vector<vector<real>>
#define INF 1e12
#include <unistd.h>

class UnionFind {

    int sz;
    vector<int> parents;
public:
    UnionFind() {
        sz = 0;
        parents.clear();
    }

    UnionFind(int nsz) {
        sz = nsz;
        parents = vector<int>(sz);
        std::iota(parents.begin(), parents.end(), 0);
    }

    int find(int i) {

        if(parents[i] == i) {
            return i;
        }
        return (parents[i] = find(parents[i]));
    }

    void merge(int i, int j){
        int pi = find(i);
        int pj = find(j);

        if (pi != pj) {
            parents[pi] = pj;
        }
    }
};


class TSP {


    int citycount;
    graph costmatrix;
    vector<int> bestTour;
    real bestCost;
    vector<int> greedyTour;
    real greedyCost;
    uniform_real_distribution<real> uni;
    mt19937 rng;
    real random01() {
        return uni(rng);
    }
public:
    TSP(graph costs, int ccount) {
        costmatrix = costs;
        citycount = ccount;

        bestTour = vector<int> (ccount);
        iota(bestTour.begin(), bestTour.end(), 0);
        bestCost = evaluateCost(bestTour, false);
        uni = uniform_real_distribution<real> (0.0,1.0);
        rng = mt19937(chrono::steady_clock::now().time_since_epoch().count());
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

    vector<int> greedyMutate(vector<int> tour) {
        vector<int> tcopy = tour;
        
        int x = random01() * citycount;
        int y = random01() * citycount;

        int l = min(x,y);
        int r = max(x,y);

        for(int i=l;i<r;++i) {
            tcopy[i] = tour[l+r-1-i];
        }
        return tcopy;
    }

    vector<int> greedyHeuristic() {

        vector<pair<pair<int,int>, real>> edges;

        for(int i=0;i<citycount;++i) {
            for(int j=0;j<i;++j) {
                edges.emplace_back(pair<int,int> (i,j),costmatrix[i][j]);
            }
        }

        auto sorter = [] (pair<pair<int,int>, real> l, pair<pair<int,int>, real> r) {
            return l.second < r.second;
        };

        stable_sort(edges.begin(), edges.end(), sorter);
        
        UnionFind uf_ds(citycount);

        vector<vector<int>> touredges(citycount, vector<int>());

        for(auto& e : edges) {

            int x,y;
            x = e.first.first;
            y = e.first.second;
            if ((uf_ds.find(x) != uf_ds.find(y)) && (touredges[x].size() < 2) && (touredges[y].size() < 2)) {
                touredges[x].push_back(y);
                touredges[y].push_back(x);
                uf_ds.merge(x,y);
            }
        }

        int fst;
        for(int i=0;i<citycount;++i) {
            if (touredges[i].size() == 1) {
                fst = i;
            }
        }

        vector<int> ans;
        ans.push_back(fst);
        vector<bool> taken(citycount, false);
        taken[fst] = true;
        int lst = fst;
        while(ans.size() < citycount) {
            for(auto n : touredges[lst]) {
                if (!taken[n]) {
                    ans.push_back(n);
                    taken[n] = true;
                    lst = n;
                }
            }
        }
        
        greedyCost = evaluateCost(ans, true);
        greedyTour = ans;
        outputTour(greedyTour);
        cout << greedyCost << '\n';
        return ans;
    }

    void outputTour(vector<int>& tour) {
        for(auto x : tour) {
            cout << x << ' ';
        }
        cout << '\n';
    }

    void adaptiveSimulatedAnnealing() {
        
        real alpha = 1200;
        real beta = 3;
        real Tinit = 1000.0;
        real Tend = 0.005;
        real Tcool = (alpha * sqrt(citycount) - 1.0)/(alpha * sqrt(citycount));
        int gmax = beta * citycount;
        real optCost = greedyCost;

        real curT = Tinit;
        vector<int> curTour(citycount);
        iota(curTour.begin(), curTour.end(), 0);
        shuffle(curTour.begin(), curTour.end(), rng);

        real curcost = evaluateCost(curTour, true);
        cout << curcost << endl;
        sleep(4);
        long long cntr = 0;
        vector<vector<int>> greedtourList;
        vector<real> greedytourCosts;
        int gcntr = 0;     
        while (curT >= Tend) {
            
            ++cntr;
            if (cntr % 100000 == 0) {
                cout << "time = " << cntr << "temp = " << curT << '\n';
                cout << curcost << '\n';
            }
            vector<int> newTour = greedyMutate(curTour);
            real newCost = evaluateCost(newTour, true);
            real diff = (newCost - curcost);

            if (diff <= 0) {
                curTour = newTour;
                curcost = newCost;
            } else {
                greedtourList.push_back(newTour);
                greedytourCosts.push_back(newCost);
                ++gcntr;
                if (gcntr < gmax) { 
                    continue;
                }
            
                int minCostpos = min_element(greedytourCosts.begin(), greedytourCosts.end()) - greedytourCosts.begin();

                newTour = greedtourList[minCostpos];
                newCost = greedytourCosts[minCostpos];
                diff = newCost - curcost;

                real rv = random01();
                real p = exp(-(diff/curT) * (10.0 * citycount / optCost));
                if (rv < p) {
                    curTour = newTour;
                    curcost = newCost;
                }
                greedtourList.clear();
                greedytourCosts.clear();
            }

            gcntr = 0;
            curT = curT * Tcool;
        }
    }
};

int main() {
    /*
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

    graph costmatrix(citycount);
    for(int i=0;i<citycount;++i) {
        for(int j=0;j<citycount;++j) {
            real x;
            cin >> x;
            costmatrix[i].push_back(x);
        }
    }


    */
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

    TSP tsp(costmatrix, citycount);
    tsp.greedyHeuristic();
    sleep(5);
    tsp.adaptiveSimulatedAnnealing();
    return 0;
}