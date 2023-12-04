#asa-gs based code
import math
import random
from load_dataset import get_costmatrix

class unionfind:
    parent_node = {}

    def initialize(self, u):
        for i in u:
            self.parent_node[i] = i

    def find(self, k):
        if self.parent_node[k] == k:
            return k
        self.parent_node[k] = self.find(self.parent_node[k])
        return self.parent_node[k]

    def union(self, a, b):
        x = self.find(a)
        y = self.find(b)
        if x != y:
            self.parent_node[x] = y

class TSP:

    def __init__(self, costmatrix, citycount):
        self.costmatrix = costmatrix
        self.citycount = citycount
        self.bestTour = []
        self.bestCost = 1000000.0 * 1000
    

    def greedyMutate(self,tour):
        N = self.citycount
        l, r = int(random.random() * N), int(random.random() * N)
        s, e = min(l,r), max(l,r)
        tcopy = tour[:]
        tcopy[s:e] = reversed(tcopy[s:e])
        return tcopy

    def computeCost(self,tour):
        graph = self.costmatrix
        cost = 0.0
        for i in range(len(tour)):
            cur, next = i, i+1
            if i + 1 == len(tour):
                next = 0
            cur = tour[i]
            next = tour[next]
            cost += graph[cur][next]
        
        if cost < self.bestCost:
            self.bestTour = tour[:]
            self.bestCost = cost
        return cost

    def greedyHeuristic(self):
        edges = []
        for  i in range(citycount):
            for j in range(i):
                edges += [[i,j,costmatrix[i][j]]]

        edges = sorted(edges, key=lambda x : x[2])
        
        uf_ds = unionfind()
        uf_ds.initialize([i for i in range(citycount)])

        touredges = dict()
        for i in range(citycount):
            touredges[i] = []

        for e in edges:
            if uf_ds.find(e[0]) != uf_ds.find(e[1]) and len(touredges[e[0]]) < 2 and len(touredges[e[1]]) < 2:
                touredges[e[0]] += [e[1]]
                touredges[e[1]] += [e[0]]
                uf_ds.union(e[0], e[1])
        
        fst = 0
        for i in range(citycount):
            if (len(touredges[i]) == 1):
                fst = i
        path = [fst]
        lst = fst
        while len(path) < citycount:
            
            for nxt in touredges[lst]:
                if nxt not in path:
                    path += [nxt]
                    lst = nxt

        self.greedyTour = path[:]
        self.greedyCost = self.computeCost(path)
        return path

    def simulatedAnnealing(self):

        N = self.citycount
        alpha = 1200
        beta = 3
        tinit = 1000.0
        tend = 0.005
        tcool = 1.0 - 1.0/(alpha * math.sqrt(N))
        gmax = beta * N
        opt = self.greedyCost

        t = tinit
        curtour = [i for i in range(citycount)]
        random.shuffle(curtour)
        curcost = self.computeCost(curtour)
        cntr = 0
        while t >= tend:

            greedList = []
            while True:
                cntr += 1
                newtour = self.greedyMutate(curtour)
                newcost = self.computeCost(newtour)
                diff = (newcost-curcost)

                if diff <= 0:
                    curtour = newtour
                    curcost = newcost
                    break

                if len(greedList) == gmax:
                    newcost, newtour = min(greedList)
                    rv = random.random()
                    diff = newcost - curcost
                    if rv <= math.exp(-(diff)/t * 10.0 * N / opt):
                        curtour = newtour
                        curcost = newcost
                    break

                greedList += [[newcost, newtour]]
            
            t = tcool * t
            print([t, curcost], sep = '\t')
    



if __name__ == '__main__':

    '''
    type = input()
    citycount = int(input())

    citypoints = []

    for i in range(0,citycount):
        l = input().strip().split(' ')
        l = list(map(float, l))
        citypoints.append(l)
    
    costmatrix = []

    for i in range(0, citycount):
        l = input().strip().split(' ')
        l = list(map(float, l))
        costmatrix.append(l)
    '''
    '''
    optpath = []
    for i in range(citycount):
        i = int(input())
        optpath += [i-1]
    
    print(f'\n\n\n {computeCost(costmatrix, optpath)}')
    greedytour = tsp.greedyHeuristic()
    greedycost = tsp.computeCost(greedytour)
    print(greedytour)
    print(greedycost)

    '''
    fname = str(input())
    citycount = int(input())
    dtype = 2
    costmatrix = get_costmatrix(fname, citycount, dtype)


    tsp = TSP(costmatrix, citycount)
    
    tsp.greedyHeuristic()
    tsp.simulatedAnnealing()