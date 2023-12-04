f = input('Enter tours file name: ')

with open(f, 'r') as t:
    cnts = t.readlines()

costs = []
lt = []
for x in cnts:
    l = x.strip().split(' ')
    lt += [int(l[0])]

print(*lt, sep=' ')