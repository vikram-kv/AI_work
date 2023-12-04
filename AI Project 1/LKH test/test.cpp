#include <bits/stdc++.h>
using namespace std;
#define real long double
#define coords pair<real, real>
#define graph vector<vector<real>>
#define INF 1e12

class UnionFind {

    int sz;
    vector<int> parents;
    int ncomp;

public:
    UnionFind() {
        sz = 0;
        parents.clear();
        ncomp = 0;
    }

    UnionFind(int nsz) {
        sz = nsz;
        parents = vector<int>(sz);
        std::iota(parents.begin(), parents.end(), 0);
        ncomp = nsz;
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
            --ncomp;
        }
    }

    int getcomp() {
        return ncomp;
    }
};


class LKH {

    graph costmatrix;
    int citycount;
    set<set<int>> curTour;
    vector<int> curAltTrail;
    vector<int> bestTour;
    real bestCost;
public:
    LKH(graph& cmat, int n) {
        this->costmatrix = cmat;
        this->citycount = n;
        curAltTrail = vector<int> {n+1,0};
        this->bestCost = INF;
    } 

    vector<int> edgesToTour(set<set<int>>& edges) {
        vector<vector<int>> v(citycount);

        for(auto e : edges) {
            vector<int> p({e.begin(), e.end()});
            int x = p[0], y = p[1];
            v[x].push_back(y);
            v[y].push_back(x);
        }

        vector<int> tour;
        vector<bool> visited(citycount, false);

        int cur = 0;
        tour.push_back(cur);
        visited[cur] = true;
        while (tour.size() != citycount) {
            for(auto x : v[cur]) {
                if (!visited[x]) {
                    tour.push_back(x);
                    visited[x] = true;
                    cur = x;
                    break;
                }
            }
        }

        return tour;
    }

    void status() {
        vector<int> tour = edgesToTour(curTour);
        real cost = evaluateCost(tour, true);
        for(auto x : tour) {
            cout << x << ' ';
        } cout << endl;
        cout << cost << '\n';
    
        cout << "BEST = ";
        for(auto x : bestTour) {
            cout << x << ' ';
        } cout << endl;
        cout << bestCost << '\n';
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

        if (bestFlag) {
            if (pathcost < bestCost) {
                bestCost = pathcost;
                bestTour = tour;
            }
        }
        return pathcost;
    }

    void initCurTourRandom() {
        vector<int> tour = vector<int>(citycount, 0);
        for(int i=0;i<citycount;++i) {
            tour[i] = i;
        }
        random_shuffle(tour.begin(), tour.end());
        curTour = tourToEdges(tour);
    }

    set<set<int>> tourToEdges(vector<int>& tour) {
        set<set<int>> edges = {};
        for(int i=0;i<citycount;++i) {
            set<int> e;
            e.insert(tour[i]);
            e.insert(tour[(i+1)%citycount]);
            edges.insert(e);
        }
        return edges; 
    }

    set<set<int>> trailToEdges(vector<int>& v, int m) {
        set<set<int>> edges = {};
        for(int i=0;i<m;++i) {
            set<int> e;
            e.insert(v[i]);
            e.insert(v[(i+1)]);
            edges.insert(e);
        }
        return edges;
    }

    vector<int> tourNeighbours(set<set<int>>& tourEdges, int cty) {
        vector<int> ans;
        for(auto t:tourEdges) {
            if (t.find(cty) != t.end())
                for(auto x : t) {
                    if (x != cty) {
                        ans.push_back(x);
                    }
                }
        }
        return ans;
    }

    vector<int> remCandidates(int i) {
        vector<int>& v = curAltTrail;
        vector<int> neighbours = tourNeighbours(curTour, v[i]);
        set<set<int>> trailEdges = trailToEdges(v, i);
        vector<int> allowed;
        for(auto n : neighbours) {
            set<int> s({v[i], n});
            if(trailEdges.find(s) == trailEdges.end()) {
                allowed.push_back(n);
            }
        }
        return allowed;
    }

    bool checkTour(vector<set<int>>& edges) {

        UnionFind ufds(citycount);
        for(auto e :edges) {
            vector<int> v(e.begin(), e.end());
            int x = v[0], y= v[1];
            ufds.merge(x,y);
        }

        return (ufds.getcomp() == 1);
    }

    real improve(int p1 = 5, int p2 = 2) {
        real g_opt = 0.0;
        int n = citycount;
        stack<vector<int>> stk;
        stack<real> gtsk;

        for(int i=0;i<n;++i) {
            vector<int> t;
            t.push_back(i); t.push_back(0);
            stk.push(t);
            gtsk.push(0.0);
        }

        vector<int>& v = curAltTrail;
        vector<set<int>> bt(curTour.begin(), curTour.end());
        while(!stk.empty()) {
            real g = gtsk.top(); gtsk.pop();
            vector<int> tp = stk.top(); stk.pop();
            int u = tp[0];
            int i = tp[1];
            v[i] = u;
            if (i%2 == 0) {
                vector<int> remAllowed = remCandidates(i);
                for(auto w : remAllowed) {

                    vector<int> t({w,i+1});
                    if (i <= p2) {
                        stk.push(t);
                        gtsk.push(g + costmatrix[v[i]][w]);
                        continue;
                    }
                    
                    set<set<int>> un = trailToEdges(v, i);
                    un.insert(set<int>({v[i], w}));
                    un.insert(curTour.begin(), curTour.end());

                    if (w != v[0]) {
                        set<set<int>> trailEdges2 = un;
                        trailEdges2.insert(set<int>({w,v[0]}));
                        vector<set<int>> sd(4*n);
                        auto itr1 = set_symmetric_difference(curTour.begin(), curTour.end(), trailEdges2.begin(), trailEdges2.end(), sd.begin());
                        sd.resize(itr1 - sd.begin());
                        if (find(un.begin(), un.end(), set<int>({v[0], w})) == un.end() && checkTour(sd)) {
                            stk.push(t);
                            gtsk.push(g + costmatrix[v[i]][w]);
                        }
                    }
                }
            } else {
                if (g > costmatrix[v[0]][v[i]] && g - costmatrix[v[0]][v[i]] > g_opt) {
                    set<set<int>> trailEdges = trailToEdges(v,i);
                    if (v[0] != v[i])
                        trailEdges.insert(set<int>({v[i], v[0]}));

                    vector<set<int>> sd(2*n);
                    auto itr = set_symmetric_difference(curTour.begin(), curTour.end(), trailEdges.begin(), trailEdges.end(), sd.begin());
                    sd.resize(itr - sd.begin());
                    if (checkTour(sd)) {
                        g_opt = g - costmatrix[v[0]][v[i]];
                        bt = sd;
                    }
                }

                set<set<int>> un = trailToEdges(v, i);
                un.insert(curTour.begin(), curTour.end());

                for(int w=0;w<n;++w) {
                    if (w == v[i]) continue;

                    if (g > costmatrix[v[i]][w] && find(un.begin(), un.end(), set<int>({v[i], w})) == un.end()) {
                        vector<int> t({w, i+1});
                        stk.push(t);
                        gtsk.push(g - costmatrix[v[i]][w]);
                    }
                }               
            }

            if (!stk.empty()) {
                g = gtsk.top();
                tp = stk.top();
                u = tp[0];
                int j = tp[1];
                if (j <= i) {
                    if (g_opt > 0) {
                        curTour = set<set<int>> (bt.begin(), bt.end());
                        return g_opt;
                    } else {
                        while (j > p1) {
                            stk.pop();
                            gtsk.pop();
                            tp = stk.top();
                            j = tp[1];
                        }
                    }
                }
            }
        }
        return g_opt;
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
        
        int greedyCost = evaluateCost(ans, true);
        curTour = tourToEdges(ans);
        cout << greedyCost << '\n';
        return ans;
    }

    void runner(int epochs = 10000, int runs=100) {

        int i=0;
        greedyHeuristic();
        while (i < runs) {
            ++i;
            int j = 0;
            while (j < epochs) {
                real r = improve();
                if (r == 0.0) break;
                status();
                ++j;
            }
            
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
   /**
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
    LKH obj(costmatrix, citycount);
    obj.runner(1000);
    return 0;
}