#include "TSP.h"

int minKey(vector<float> key, vector<bool> mstSet, int V)
{
    // Initialize min value
    int min = INT_MAX, min_index;
 
    for (int v = 0; v < V; v++)
        if (mstSet[v] == false && key[v] < min)
            min = key[v], min_index = v;
 
    return min_index;
}

vector<int> TSP::christofides() {
    int V = n;
    vector<int> path = vector<int>();

    // Array to store constructed MST
    vector<int> parent(V);
     
    // Key values used to pick minimum weight edge in cut
    vector<float> key(V);
     
    // To represent set of vertices included in MST
    vector<bool> mstSet(V);
 
    // Initialize all keys as INFINITE
    for (int i = 0; i < V; i++)
        key[i] = INFINITY, mstSet[i] = false;
 
    // Always include first 1st vertex in MST.
    // Make key 0 so that this vertex is picked as first vertex.
    key[0] = 0;
    parent[0] = -1; // First node is always root of MST
 
    // The MST will have V vertices
    for (int count = 0; count < V; count++)
    {
        // Pick the minimum key vertex from the
        // set of vertices not yet included in MST
        int u = minKey(key, mstSet, V);
 
        // Add the picked vertex to the MST Set
        mstSet[u] = true;
        // path.push_back(u);
 
        // Update key value and parent index of
        // the adjacent vertices of the picked vertex.
        // Consider only those vertices which are not
        // yet included in MST
        for (int v = 0; v < V; v++)
 
            // mat[u][v] is non zero only for adjacent vertices of m
            // mstSet[v] is false for vertices not yet included in MST
            // Update the key only if mat[u][v] is smaller than key[v]
            if (mat[u][v] && mstSet[v] == false && mat[u][v] < key[v])
                parent[v] = u, key[v] = mat[u][v];
    }



    // construct a tree by forming adjacency matrices
    adjlist = vector<vector<int>> (n);
    for (int i = 0; i < n; i++) {
        int j = parent[i];
        if (j != -1) {
            adjlist[i].push_back(j);
            adjlist[j].push_back(i);
        }
    }

    // find a perfect matching
    int closest, length;
    vector<int>::iterator tmp, first;
    //find nodes with odd degree
    odds.clear();
    for (int i = 0; i < n; i++) {
        // if degree of vertex i is odd
        if ((adjlist[i].size() % 2) != 0) {
            // push vertex to odds, which is a representation of subgraph O
            odds.push_back(i);
        }
    }
    while (!odds.empty()) {
        first = odds.begin();
        vector<int>::iterator it = odds.begin() + 1;
        vector<int>::iterator end = odds.end();
        length = std::numeric_limits<int>::max();
        for (; it != end; ++it) {
            // if this node is closer than the current closest, update closest and length
            if (mat[*first][*it] < length) {
                length = mat[*first][*it];
                closest = *it;
                tmp = it;
            }
        } // two nodes are matched, end of list reached
        adjlist[*first].push_back(closest);
        adjlist[closest].push_back(*first);
        odds.erase(tmp);
        odds.erase(first);
    }

    float target = INFINITY;
    for(int i = 0; i < n; i++) {
        vector<int> temp;
        eulertour(i, temp);
        float tcost;
        makehamilton(temp, tcost);
        if(tcost < target) {
            target = tcost;
            path = temp;
        }
    }

    return path;
}

void TSP::eulertour(int start, vector<int>& path) {
    vector<int> *tempList = new vector<int>[n];
	for(int i = 0; i < n; i++){
		tempList[i].resize(adjlist[i].size());
		tempList[i] = adjlist[i];
	}

	stack<int> stack;
	int pos = start;
	path.push_back(start);
	while(!stack.empty() || tempList[pos].size() > 0){
		//Current node has no neighbors
		if(tempList[pos].empty()){
			//add to circuit
			path.push_back(pos);
			//remove last vertex from stack and set it to current
			pos = stack.top();
			stack.pop();
		}
		//If current node has neighbors
		else{
			//Add vertex to stack
			stack.push(pos);
			//Take a neighbor
			int neighbor = tempList[pos].back();
			//Remove edge between neighbor and current vertex
			tempList[pos].pop_back();
			for(int i = 0; i < tempList[neighbor].size(); i++){
				if(tempList[neighbor][i] == pos){
					tempList[neighbor].erase(tempList[neighbor].begin()+i);
				}
			}
			//Set neighbor as current vertex
			pos = neighbor;
		}
	}
	path.push_back(pos);

}

void TSP::makehamilton(vector<int>& path, float& cost) {
    vector<bool> visited = vector<bool>(n, false);
	
	cost = 0;

	int root = path.front();
	vector<int>::iterator cur = path.begin();
	vector<int>::iterator iter = path.begin()+1;
	visited[root] = true;

	//iterate through circuit
	bool addMore = true;
	while(iter != path.end()){
		if(!visited[*iter]){
			cost += mat[*cur][*iter];
			cur = iter;
			visited[*cur] = true;
			iter = cur + 1;
		}	
		else{
			iter = path.erase(iter);
		}
	}
	//Add distance to root
	if ( iter != path.end() ){
		cost += mat[*cur][*iter];
	}
}