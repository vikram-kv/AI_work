#include "tsp.h"

real fitness(graph& costmatrix, int citycount, vector<int>& tour) {
        real pathcost = 0;
        for(int i=0;i<citycount;++i) {
            int cur = tour[i], next;
            if (i+1 < citycount) 
                next = tour[i+1];
            else 
                next = tour[0];
            pathcost += costmatrix[cur][next];
        }
        return pathcost;
}

void fixcity(vector<int>& tour) {
    if (*tour.begin() == 0) return ;
    while(*(tour.begin())!=0) {
        int x = *(tour.begin());
        tour.erase(tour.begin());
        tour.push_back(x);
    }
}

bool checkvalidity(vector<int>& tour, int ccount) {
    if (tour.size() != ccount) return false;
    vector<bool> vis(ccount, false);
    for(int i=0;i<ccount;++i) {
        vis[tour[i]] = true;
    }
    for(auto v : vis) {
        if (!v) {
            return false;
        }
    }
    return true;
}

real random01() {
    std::random_device rd;
    std::default_random_engine eng(rd());
    std::uniform_real_distribution<real> distr(0, 1);

    return distr(eng);
}
