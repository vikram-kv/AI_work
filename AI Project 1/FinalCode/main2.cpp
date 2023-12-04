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
    real random01() {
        return ((real)rand())/RAND_MAX;
    }
    int randmod(int x) {
        return rand() % x;
    }
public:
    TSP(graph costs, int ccount) {
        costmatrix = costs;
        citycount = ccount;

        bestTour = vector<int> (ccount);
        iota(bestTour.begin(), bestTour.end(), 0);
        bestCost = evaluateCost(bestTour, false);
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
        
        int x = randmod(citycount);
        int y = randmod(citycount);

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

    void simulatedAnnealing() {
        // temp initializations need to be finetuned...
        real Tinit = 1000.0;
        real Tend = 1e-8;
        real Tcool = 0.998; // hit tend -> repeat tend for several cycles
        int M = 100;
        real curT = Tinit;
        vector<int> curTour(citycount);
        iota(curTour.begin(), curTour.end(), 0);
        shuffle(curTour.begin(), curTour.end(), std::random_device());
        real curcost = evaluateCost(curTour, true);
        long long cntr = 0;  
        while (curT >= Tend) {
            
            ++cntr;
            if (cntr % 100 == 0) {
                cout << "time = " << cntr << "temp = " << curT << '\n';
                cout << curcost << '\n';
            }

            int i = 0;
            while (i < M) {
                vector<int> newTour = greedyMutate(curTour);
                real newCost = evaluateCost(newTour, true);
                real diff = (newCost - curcost);
                if (diff <= 0) {
                    curTour = newTour;
                    curcost = newCost;
                } else {
                    real rv = random01();
                    real p = exp(-(diff/curT));         // here, think about scale factors
                    if (rv < p) {
                        curTour = newTour;
                        curcost = newCost;
                    }
                }
                ++i;
            }
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

    TSP tsp(costmatrix, citycount);
    tsp.greedyHeuristic();
    tsp.simulatedAnnealing();
    return 0;
}