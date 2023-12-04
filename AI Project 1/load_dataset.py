import math

def get_costmatrix(fname, citycount, dtype):
    n = citycount
    f = open(fname, 'r')
    cnts = f.readlines()
    f.close()

    numbers = []
    for l in cnts:
        l = l.strip()
        if any(c.isalpha() for c in l):
            continue
        l = list(map(float,l.split()))
        numbers += l

    if dtype == 1:
        costmatrix = [[-1 for j in range(n)] for i in range(n)]

        i = 0
        c = 0
        while i < n:
            j = 0
            while j <= i:
                costmatrix[i][j] = costmatrix[j][i] = numbers[c]
                j += 1
                c += 1
            i += 1

        return costmatrix
    
    elif dtype == 2: 
        coords = []
        for i in range(0,3*citycount,3):
            coords += [(numbers[i+1], numbers[i+2])]

        costmatrix = [[-1 for j in range(n)] for i in range(n)]

        i = 0
        while i < n:
            j = 0
            while j <= i:
                cost = math.sqrt((coords[i][0] - coords[j][0]) ** 2 + (coords[i][1] - coords[j][1]) ** 2 )
                costmatrix[i][j] = costmatrix[j][i] = cost
                j += 1
            i += 1

        return costmatrix


def cppfilegen(fname, citycount, dtype):
    costmatrix = get_costmatrix(fname, citycount, dtype)


    outname = fname.split('.')[0] + '.ctsp'

    f = open(outname, 'w')
    f.write(str(len(costmatrix)))
    f.write('\n')
    for i in costmatrix:
        for j in i:
            f.write(str(j))
            f.write(' ')
        f.write('\n')
    f.close()

if __name__ == '__main__':


    fname = input()
    citycount = int(input())
    dtype = int(input())
    cppfilegen(fname, citycount, dtype)
