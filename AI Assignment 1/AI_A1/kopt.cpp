#include "tsp.h"

KOpt::KOpt() {
    costmatrix = {};
    citycount = 0;
    epsilon = 0.1;
}

KOpt::KOpt(graph& costs, int ccount) {
    costmatrix = costs;
    citycount = ccount;
    epsilon = 0.1;
}

vector<int> KOpt::two_opt(vector<int>& srctour) {
    bool cont = true;
    int n = srctour.size();
    vector<int> ans = srctour;
    real curcost = fitness(costmatrix, citycount, ans);
    int cntr = 1;
    int u1, u2, v1, v2;
    while(cont) {
        cont = false;
        for(int i=0;i<=n-2;++i) {
            for(int j=i+1;j<=n-1;++j) {
                // we check if swapping (i,i+1), (j,j+1) edges with (i,j) and (i+1,j+1) edges is better
                u1 = ans[i]; u2 = ans[(i+1)%n];
                v1 = ans[j]; v2 = ans[(j+1)%n];
                real savings = costmatrix[u1][u2] + costmatrix[v1][v2] - costmatrix[u1][v1] - costmatrix[u2][v2];
                if (savings > epsilon) {
                    curcost -= savings;
                    cont = true;
                    two_optify(ans, i, j);
                }
            }
        }
    }
    return ans;
}

void KOpt::two_optify(vector<int>& tour, int i, int j) {
    reverse(tour.begin() + i + 1, tour.begin() + j + 1);
}

vector<int> KOpt::three_opt(vector<int>& srctour) {
    bool cont = true;
    int n = srctour.size();
    vector<int> ans = srctour;
    real curcost = fitness(costmatrix, citycount, ans);
    while(cont) {
        cont = false;
        for(int i=1;i<n-4;++i) {
            for(int j=i+2;j<n-2;++j) {
                for(int k=j+2;k<n;++k) {
                    if (three_optify(ans, i, j, k)) 
                        cont = true;
                }
            }
        }
    }
    return ans;
}

bool KOpt::three_optify(vector<int>& tour, int i, int j, int k) {
    int a, b, c, d, e, f;
    int n = tour.size();
    a = tour[i-1]; b = tour[i]; 
    c = tour[j-1]; d = tour[j];
    e = tour[k-1]; f = tour[k];
    // our tour is (ab).... (cd).... (ef) ....

    real d_orig = costmatrix[a][b] + costmatrix[c][d] + costmatrix[e][f];
    // case 1 -> (ab) edge is retained
    real d_1 = costmatrix[a][b] + costmatrix[c][e] + costmatrix[d][f];
    // case 2 -> (cd) edge is retained
    real d_2 = costmatrix[a][e] + costmatrix[d][c] + costmatrix[b][f];
    // case 3 -> (ef) edge is retained
    real d_3 = costmatrix[a][c] + costmatrix[b][d] + costmatrix[e][f];
    // case 4 -> no edge is retained. here, 4 cases are possible. But we choose only 1 for performance
    real d_4 = costmatrix[a][d] + costmatrix[e][b] + costmatrix[c][f];

    if (d_1 + epsilon < d_orig) {
        // reverse segment between j and k to ensure we have ce and df edges in our tour
        reverse(tour.begin() + j, tour.begin() + k);
        return true;
    } else if (d_2 + epsilon < d_orig) {
        // reverse segment between i and k to ensure we have ae and bf edges in our tour
        reverse(tour.begin() + i, tour.begin() + k);
        return true;
    } else if (d_3 + epsilon < d_orig) {
        // reverse segment between i and j to ensure we have ac and bd edges in our tour
        reverse(tour.begin() + i, tour.begin() + j);
        return true;
    } else if (d_4 + epsilon < d_orig) {
        vector<int> ntour;
        // add portion between j and k
        for(auto itr = tour.begin() + j; itr < tour.begin() + k; ++itr) {
            ntour.push_back(*itr);
        }

        // add portion between i and j next
        for(auto itr = tour.begin() + i; itr < tour.begin() + j; ++itr) {
            ntour.push_back(*itr);
        }

        // add portion between k and i finally
        for(auto itr = tour.begin() + k; itr<tour.end();++itr) {
            ntour.push_back(*itr);
        }

        for(auto itr = tour.begin(); itr<tour.begin() + i;++itr) {
            ntour.push_back(*itr);
        }
        tour = ntour;
        return true;
    }

    return false;
}