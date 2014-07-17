#! /usr/bin/python

import sys
import os
import re

cc = cxx = build = None

for arg in sys.argv[1:]:
    if arg.startswith('--cc='):
        cc = arg.split('=')[1]
    elif arg.startswith('--cxx='):
        cxx = arg.split('=')[1]
    else:
        build = arg

if (cc or cxx) and build:
    for fname in os.listdir(build):
        if fname.endswith('.make'):
            path = os.path.join(build, fname)
            with open(path) as f:
                old = f.read()
            with open(path, 'w+') as f:
                if cc:
                    f.write('CC = %s\n' % cc)
                if cxx:
                    f.write('CXX = %s\n' % cxx)
                f.write(old)
else:
    print >> sys.stderr, 'post_premake.py --cc=gcc --cxx=g++ build'
