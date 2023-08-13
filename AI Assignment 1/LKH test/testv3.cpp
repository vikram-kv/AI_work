#include <bits/stdc++.h>
using namespace std;
#define real long double
#define coords pair<real, real>
#define graph vector<vector<real>>
#define INF 1e12
#define edge pair<int,int>

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

class TSP {

    graph costmatrix;
    int citycount;
    vector<int> bestTour;
    real bestCost;

    // for LKH implementation
    set<set<edge>> lkhSolutions;
    set<edge> curSolution;
    vector<int> curTour;
    real curCost;
    int runcounter;

    edge edger(int x, int y) {
        if (x < y) return edge {x,y};
        return edge {y,x};
    }

public:
    TSP(graph& cmat, int n) {
        this->costmatrix = cmat;
        this->citycount = n;
        this->bestCost = INF;
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

    set<edge> tourToEdges(vector<int>& tour) {
        set<edge> edges = {};
        for(int i=0;i<citycount;++i) {
            edge e = edger(tour[i], tour[(i+1)%citycount]);
            edges.insert(e);
        }
        assert(edges.size() == citycount);
        return edges; 
    }

    vector<int> edgesToTour(set<edge>& edges) {
        vector<vector<int>> v(citycount);

        for(auto e : edges) {
            int x = e.first, y = e.second;
            v[x].push_back(y);
            v[y].push_back(x);
        }

        // cout << "RUN " << runcounter << '\n';
        // cout << "DEBUG " << v.size() << '\n';
        // for(auto a : v) {
        //     cout << a.size() << '\t';
        //     assert(a.size() == 2);
        //     if (a.size() != 2) {
        //         cout << "check \n";
        //         for(auto x : a) {
        //             cout << x << ' ';
        //         }
        //         cout << '\n';
        //     }
        // }

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
        assert(tour.size() == citycount);
        return tour;
    }

    set<int> adjVertices(set<edge>& tour, int v) {
        set<int> answer = {};
        for(auto e : tour) {
            if (e.first == v) {
                answer.insert(e.second);
            }
            if (e.second == v) {
                answer.insert(e.first);
            }
        }
        answer.erase(v);
        //assert(answer.size() == 2);
        return answer;
    }

    set<int> adjVertices(vector<int>& tour, int v) {
        set<int> answer = {};
        int n = citycount;
        int i;
        for(i=0;i<n;++i) {
            if (tour[i] == v) break;
        }
        answer.insert(tour[(n+i-1)%n]);
        answer.insert(tour[(i+1)%n]);
        answer.erase(v);
        //assert(answer.size() == 2);
        return answer;
    }

    vector<pair<int,pair<real, real>>> bestNeighbours(set<edge>& tour, int lv, real gain, set<edge>& addedEdges, set<edge>& removedEdges) {

        map<int, pair<real, real>> answer = {};
        int n = citycount;
        for(int i=0;i<n;++i) {
            if (i == lv) continue;
            edge candEdge = edger(i, lv);
            real candGain = gain - costmatrix[i][lv];
            if (candGain <= 0 || removedEdges.find(candEdge) != removedEdges.end() || tour.find(candEdge) != tour.end()) continue;
            for(auto next : adjVertices(tour, i)) {
                edge nextEdge = edger(next,i);
                if (removedEdges.find(nextEdge) == removedEdges.end() && addedEdges.find(nextEdge) == addedEdges.end()) {
                    real possGain = costmatrix[next][i] - costmatrix[lv][i];
                    if (answer.find(i) == answer.end() || answer[i].first < possGain) {
                        answer[i] = pair<real,real>{possGain, candGain};
                    }
                }
            }
        }

        vector<pair<int, pair<real,real>>> ans(answer.begin(), answer.end());
        auto sortKey = [] (pair<int, pair<real, real>> l, pair<int, pair<real, real>> r) {
            return l.second.first > r.second.first;
        };
        sort(ans.begin(), ans.end(), sortKey);
        return ans;
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
        cout << "GREEDY :)" << greedyCost << '\n';
        return ans;
    }

    void lkh_runner() {
        bool cont = true;
        lkhSolutions.clear();
        // curTour = vector<int> (citycount,0);
        // iota(curTour.begin(), curTour.end(), 0);
        // random_shuffle(curTour.begin(), curTour.end());
        // curSolution = tourToEdges(curTour);

        curTour = greedyHeuristic();
        curSolution = tourToEdges(curTour);
        cout << "RUNNING \n\n\n";
        runcounter = 0;
        while(cont) {
            ++runcounter;
            cont = lkh_boost();
            curTour = edgesToTour(curSolution);
            evaluateCost(curTour, true);
            status();
            lkhSolutions.insert(curSolution);
        }
    }

    void status() {
        real cost = evaluateCost(curTour, true);
        for(auto x : curTour) {
            cout << x << ' ';
        } cout << endl;
        cout << cost << '\n';
    
        cout << "BEST = ";
        for(auto x : bestTour) {
            cout << x << ' ';
        } cout << endl;
        cout << bestCost << '\n';
    }

    bool lkh_boost() {
        vector<int> tour = curTour;
        set<edge> tourEdges = curSolution;

        for(auto u : tour) {
            auto t =  adjVertices(tour, u);
            for(auto v : t) {
                set<edge> removedEdges;
                removedEdges.insert(edger(u,v));

                set<edge> addedEdges;
                real gain = costmatrix[u][v];
                auto bestn = bestNeighbours(tourEdges, v, gain, addedEdges, removedEdges);
                int iter = 5;

                for(auto& n : bestn) {
                    real gainn = n.second.second;
                    int w = n.first;
                    if (t.find(w) != t.end()) continue;

                    addedEdges.insert(edger(v,w));

                    if (removeEdge(tourEdges, u, w, gainn, addedEdges, removedEdges))
                        return true;
                    
                    iter--;
                    if (iter == 0) break;
                }
            }
        }
        return false;
    }

    pair<bool, set<edge>> makeTour(set<edge>& tour, set<edge>& addedEdges, set<edge>& removedEdges) {

        int n = citycount;
        vector<edge> t(2*n);
        auto itr = set_difference(tour.begin(), tour.end(), removedEdges.begin(), removedEdges.end(), t.begin());
        t.resize(itr-t.begin());

        vector<edge> newtour(2*n);
        itr = set_union(t.begin(), t.end(), addedEdges.begin(), addedEdges.end(), newtour.begin());
        newtour.resize(itr-newtour.begin());

        if (newtour.size() < n) {
            return {false, {}};
        }

        vector<vector<int>> v(n);

        for(auto e : newtour) {
            int x = e.first, y = e.second;
            v[x].push_back(y);
            v[y].push_back(x);
        }

        vector<int> ntour;
        vector<bool> visited(citycount, false);

        int cur = 0;
        ntour.push_back(cur);
        visited[cur] = true;
        bool ok;
        while (ntour.size() != citycount) {
            ok = false;
            for(auto x : v[cur]) {
                if (!visited[x]) {
                    ok = true;
                    ntour.push_back(x);
                    visited[x] = true;
                    cur = x;
                    break;
                }
            }
            if (!ok) break;
        }

        return {ok, set<edge>(newtour.begin(), newtour.end())};
    }

    bool removeEdge(set<edge>& tourEdges, int fv, int lv, real gain, set<edge>& addedEdges, set<edge>& removedEdges) {

        set<int> rmCands = adjVertices(tourEdges, lv);
        if (removedEdges.size() == 4) {
            int u, v;
            vector<int> x(rmCands.begin(), rmCands.end());
            u = x[0]; v = x[1];

            if (costmatrix[lv][u] > costmatrix[lv][v]) {
                rmCands.erase(v);
            } else {
                rmCands.erase(u);
            }
        }

        for(auto n : rmCands) {
            edge rmEdge(edger(lv, n));
            real gainn = gain + costmatrix[lv][n];

            if (addedEdges.find(rmEdge) != addedEdges.end() || removedEdges.find(rmEdge) != removedEdges.end()) continue;
            
            set<edge> rmedgesc = removedEdges;
            rmedgesc.insert(rmEdge);
            set<edge> aadedgesc = addedEdges;
            aadedgesc.insert(edger(n,fv));
            real linkgain = gainn - costmatrix[fv][n];
            auto t = makeTour(tourEdges, aadedgesc, rmedgesc);
            bool is_tour = t.first;
            auto newSolution = t.second;
            if (! is_tour && aadedgesc.size() > 2) continue;

            if (lkhSolutions.find(newSolution) != lkhSolutions.end())
                return false;
            
            if (is_tour && linkgain > 0) {
                curSolution = newSolution;
                return true;
            } else {
                return addEdge(tourEdges, fv, n, gainn, rmedgesc, addedEdges);
            }
        }
        return false;
    }

    bool addEdge(set<edge>& tourEdges, int fv, int lv, real gain, set<edge>& addedEdges, set<edge>& removedEdges) {

        vector<pair<int,pair<real,real>>> bestGains = bestNeighbours(tourEdges, lv, gain, addedEdges, removedEdges);
        int iter = 1;
        if (removedEdges.size() == 2) {
            iter = 5;
        }

        for(auto& n : bestGains) {
            real gainn = n.second.second;
            set<edge> nadded(addedEdges);
            nadded.insert(edger(n.first, lv));
            if (removeEdge(tourEdges, fv, n.first, gainn, removedEdges, nadded)) {
                return true;
            }
            --iter;
            if (iter == 0) return false;
        }
        return false;
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
    TSP obj(costmatrix, citycount);
    obj.lkh_runner();
    return 0;
}