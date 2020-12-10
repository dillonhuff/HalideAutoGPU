#!/usr/bin/python3

import re

f = open('res.txt', 'r').readlines()

p = re.compile('Load (.*)\((.*)\) = .*')

component_mins = []
for i in range(10):
    component_mins.append(999999)

component_maxs = []
for i in range(10):
    component_maxs.append(-999999)

for l in f:
    # print(l)
    res = re.match(p, l)
    if res:
        # print('\tMatched:', res[2])
        pos = 0
        for val in res[2].split(','):
            iv = int(val)
            # print('\t\t', iv)
            component_maxs[pos] = max(iv, component_maxs[pos])
            component_mins[pos] = min(iv, component_mins[pos])
            pos += 1

print('Mins...')
for c in component_mins:
    print('\t', c)

print('Maxs...')
for c in component_maxs:
    print('\t', c)



