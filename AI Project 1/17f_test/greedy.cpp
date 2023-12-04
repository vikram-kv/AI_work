#include "tsp.h"

UnionFind::UnionFind() {
    sz = 0;
    parents.clear();
}

UnionFind::UnionFind(int nsz) {
    sz = nsz;
    parents = vector<int>(sz);
    std::iota(parents.begin(), parents.end(), 0);
}

int UnionFind::find(int i) {

    if(parents[i] == i) {
        return i;
    }
    return (parents[i] = find(parents[i]));
}

void UnionFind::merge(int i, int j){
    int pi = find(i);
    int pj = find(j);

    if (pi != pj) {
        parents[pi] = pj;
    }
}

GreedyHeuristic::GreedyHeuristic() {
    costmatrix = {};
    citycount = 0;
}

GreedyHeuristic::GreedyHeuristic(graph& costs, int ccount) {
    costmatrix = costs;
    citycount = ccount;
}

void  GreedyHeuristic::runner() {

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
    greedyTour = ans;
}

vector<int> GreedyHeuristic::getGreedyTour() {
    return greedyTour;
}