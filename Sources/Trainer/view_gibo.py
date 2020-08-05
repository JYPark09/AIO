import sys

import numpy as np

if len(sys.argv) != 2:
    print('usage: python3 {} path'.format(sys.argv[0]))
    sys.exit()

with open(sys.argv[1]+'/prob') as f:
    for line in f.readlines():
        prob = np.fromstring(line, dtype=np.float, sep=' ')

        action = prob.argmax()

        x = action % 8
        y = action // 8 + 1

        print(chr(ord('a')+x) + str(y), end='')\
        
    print()
