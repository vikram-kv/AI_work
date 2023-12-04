import random
import math
from time import sleep
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
        #print(e)
        if uf_ds.find(e[0]) != uf_ds.find(e[1]) and len(touredges[e[0]]) < 2 and len(touredges[e[1]]) < 2:
            touredges[e[0]] += [e[1]]
            touredges[e[1]] += [e[0]]
            uf_ds.union(e[0], e[1])

    for i in range(citycount):
        print(touredges[i])

    fst = -1
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



def simulatedAnnealing(costmatrix, epochs=1000000, tstart=100, tstop=1e-8, runs=1000, cf=0.996):

    for i in range(runs):
        print(f'run {i}\n\n\n')
        cur_tour = bestTour[:]
        cur_cost = computeCost(costmatrix, cur_tour)
        cur_t = tstart
        for j in range(epochs):
            if j % 20 == 0:                        
                print(bestCost)
                print(bestTour)
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


def createRandomPopulation(ncities, popsize):
    population = []
    l = [i for i in range(ncities)]
    for i in range(popsize):
        random.shuffle(l)
        population.append(l[:])
    return population

def selectOnFitness(graph, population, eliteSize):
    popsize = len(population)
    popfitnesslist = []
    for p in population:
        popfitnesslist.append((p[:], 1/computeCost(graph, p)))
    
    popfitnesslist = sorted(popfitnesslist, key=lambda x: x[1], reverse=True)
    sortedpop, fitnessvalues = zip(*popfitnesslist)
    sortedpop = list(sortedpop)
    fitnessvalues = list(fitnessvalues)
    elitepop = sortedpop[0:eliteSize]
    
    total_fitness = 0.0
    for f in fitnessvalues:
        total_fitness += f
    
    cum_prob = fitnessvalues[:]
    for i in range(1,popsize):
        cum_prob[i] += cum_prob[i-1]
    
    cum_prob = [cum_prob[i]/total_fitness * 100 for i in range(popsize)]
    fitnesspop = []
    for i in range(eliteSize, popsize):
        rv = random.random() * 100.0
        for j in range(popsize):
            if cum_prob[j] > rv:
                fitnesspop.append(sortedpop[j])
                break
    
    return (elitepop, fitnesspop)


def pmxcrossover(par1, par2):
    ncities = len(par1) 
    l, r = int(random.random() * ncities), int(random.random() * ncities)

    s, e = min(l,r), max(l,r)

    child1 = [None for i in range(ncities)]
    child2 = child1[:]

    for i in range(s,e):
        child1[i] = par1[i]
        child2[i] = par2[i]
    
    map1 = dict()
    map2 = dict()

    for i in range(ncities):
        map1[par1[i]] = i
        map2[par2[i]] = i
    
    for i in range(ncities):
        if par2[i] not in child1:
            j = i
            while child1[j] != None:
                j = map2[child1[j]]
            child1[j] = par2[i]

        if par1[i] not in child2:
            j = i
            while child2[j] != None:
                j = map1[child2[j]]
            child2[j] = par1[i]
    
    return child1, child2
    
def ordercrossover(par1, par2):
    ncities = len(par1)
    l, r = int(random.random() * ncities), int(random.random() * ncities)
    
    s, e = min(l,r), max(l,r)

    child1 = [None for _ in range(ncities)]
    child2 = child1[:]

    for i in range(s,e):
        child1[i] = par1[i]
        child2[i] = par2[i]

    child2inh = par2[s:e]
    pos = 0
    for g in par1:
        if g not in child2inh:
            while child2[pos] != None:
                pos += 1
            child2[pos] = g
    
    child1inh = par1[s:e]
    pos = 0
    for g in par2:
        if g not in child1inh:
            while child1[pos] != None:
                pos += 1
            child1[pos] = g
    
    return child1, child2


def createChildPopulation(elitepop, fitnesspop):
    random.shuffle(fitnesspop)
    childpop = elitepop
    for i in range(0, len(fitnesspop), 2):
        c1, c2 = ordercrossover(fitnesspop[i], fitnesspop[i+1])
        childpop += [c1,c2]
    
    return childpop

def mutate(tour):
    i = int(random.random() * len(tour))
    j = int(random.random() * len(tour))

    t = tour[i]
    tour[i] = tour[j]
    tour[j] = t
    
    return tour

def addMutations(population, mutRate):
    for i in range(len(population)):
        rv = 100.0 * random.random()
        if rv <= mutRate:
            population[i] = mutate(population[i])
    return population


def geneticAlgo(citycount, costmatrix, epochs=600, mutRate=1.0, superepochs = 1):

    popSize = max(2000, 16 * citycount)
    eliteSize = int(popSize/40)
    if eliteSize % 2 == 1:
        eliteSize += 1
    for _ in range(superepochs):
        population = createRandomPopulation(citycount, popSize)
        for _ in range(epochs):
            elitepopulation, fitnesspopulation = selectOnFitness(costmatrix, population, eliteSize)
            population = createChildPopulation(elitepopulation, fitnesspopulation)
            population = addMutations(population, mutRate)
            print(bestTour)
            print(bestCost)


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
    optpath = []
    for i in range(citycount):
        i = int(input())
        optpath += [i-1]
    
    print(f'\n\n\n {computeCost(costmatrix, optpath)}')


    '''
    fname = str(input())
    citycount = int(input())
    dtype = 2
    costmatrix = get_costmatrix(fname, citycount, dtype)

    # init best tour and best cost using greedy heuristic
    bestCost = 10000.0 * citycount
    
    greedysol = greedyHeuristic(costmatrix, citycount)
    greedycost = computeCost(costmatrix, greedysol)
    print(bestTour)
    print(bestCost)
    #geneticAlgo(citycount, costmatrix, epochs=2000)
    #simulatedAnnealing(costmatrix)