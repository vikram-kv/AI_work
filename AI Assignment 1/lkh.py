from mimetypes import init
import random
import math
from load_dataset import get_costmatrix

bestCost = 1000000.0 * 1000
bestTour = []

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


def greedyHeuristic(costmatrix, citycount):
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

    return path
        

def computeCost(graph, tour):
    cost = 0.0
    for i in range(len(tour)):
        cur, next = i, i+1
        if i + 1 == len(tour):
            next = 0
        cur = tour[i]
        next = tour[next]
        cost += graph[cur][next]

    global bestCost
    global bestTour
    if (cost < bestCost):
        bestTour = tour[:]
        bestCost = cost

    return cost

def initsimulatedAnnealing(tour, costmatrix, epochs=1000, tstart=400, tstop=0.25, cf=0.99):

    cur_tour = tour[:]
    cur_cost = computeCost(costmatrix, cur_tour)
    cur_t = tstart
    for j in range(epochs):
        l, r = int(random.random() * len(cur_tour)), int(random.random() * len(cur_tour))
        s, e = min(l,r), max(l,r)
        new_tour = cur_tour[:]
        new_tour[s:e] = reversed(new_tour[s:e])

        new_cost = computeCost(costmatrix, new_tour)
        if new_cost < cur_cost:
            cur_tour = new_tour
            cur_cost = new_cost
            continue
        
        delta = -(new_cost - cur_cost)
        r = random.random()
        if r < math.exp(delta/cur_t):
            cur_tour = new_tour
            cur_cost = new_cost
            continue
        
        cur_t = cur_t * cf
        if cur_t < tstop:
            break
    
    return cur_tour

def lkh_boost(costmatrix, tour):
    pass

def lkh(citycount, costmatrix, epochs):
    
    i = 0
    tour = random.shuffle([i for i in range(citycount)])
    while i < epochs:
        tour = lkh_boost(costmatrix,tour[:])
        i += 1

    return tour

if __name__  == '__main__':

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



    '''
    fname = str(input())
    citycount = int(input())
    dtype = 2
    costmatrix = get_costmatrix(fname, citycount, dtype)
    optpath = []
    for i in range(citycount):
        i = int(input())
        optpath += [i-1]
    
    bestCost = 10000.0 * citycount
    greedysol = greedyHeuristic(costmatrix, citycount)
    greedycost = computeCost(costmatrix, greedysol)
    print('greedy \n\n\n')
    print(greedysol)
    print(greedycost)

