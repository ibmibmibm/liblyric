s = set()
for line in open('log','r'):
    addr = line[3:-1]
    if line[0] == '+':
        if addr in s:
            print 'leak in', addr
        s.add(addr)
    else:
        if addr not in s:
            print 'double free in', addr
        else:
            s.remove(addr)
    #print s
