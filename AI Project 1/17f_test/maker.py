import os

def extractcount(f):
    s = ''
    for c in f:
        if (c in [str(i) for i in range(0,10)]):
            s += c
    return int(s)

path = 'TSP_DATA'
dir = os.listdir(path)
print(dir)

files = []
for f in dir:
    l = f.split('.')
    if (l[1] == 'tsp'):
        files += [l[0]]

print(sorted(files))

for f in files:
    fl = 'temp.in'
    outptr = open(fl, 'w')
    outptr.write(f'{path}/{f}.tsp\n{extractcount(f)}\n2\n')
    outptr.close()
    os.system('python3 load_dataset.py < temp.in')
    os.system(f'mv {path}/{f}.ctsp ./')