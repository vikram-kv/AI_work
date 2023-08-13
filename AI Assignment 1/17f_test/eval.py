def computeCost(graph, tour):
    cost = 0.0
    for i in range(len(tour)):
        cur, next = i, i+1
        if i + 1 == len(tour):
            next = 0
        cur = tour[i]
        next = tour[next]
        cost += graph[cur][next]
    return cost


f = input('Enter tsp source file name: ')

with open(f, 'r') as t:
    cnts = t.readlines()

type = cnts[0].strip()
N = int(cnts[1].strip())

coords = []

for i in range(N):
    l = cnts[i+2].strip().split(' ')
    l = list(map(float, l))
    coords += [l]

costmatrix = []

for i in range(N):
    l = cnts[i+N+2].strip().split(' ')
    l = list(map(float, l))
    costmatrix += [l]

f = input('Enter tours file name: ')

with open(f, 'r') as t:
    cnts = t.readlines()

costs = []
for x in cnts:
    l = x.strip().split(' ')
    l = list(map(int, l))
    costs += [computeCost(costmatrix, l)]

print(costs)

