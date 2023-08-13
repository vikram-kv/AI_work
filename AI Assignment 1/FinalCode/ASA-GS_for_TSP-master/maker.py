f = input()
f = open(f, 'r')
cnts = f.readlines()
f.close()

for i in cnts:
    i = i.strip().split(' ')
    x = []
    for j in range(len(i)):
        i[j] = i[j].strip()
        if i[j] != '':
            x += [i[j]]
    print(*x, sep=' ')