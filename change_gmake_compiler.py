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

def compiler_repl(match):
    if match.group(1) == 'CC':
        return cc and '%s%s%s' % (match.group(1), match.group(2), cc) or match.group(0)
    else:
        return cxx and '%s%s%s' % (match.group(1), match.group(2), cxx) or match.group(0)

if (cc or cxx) and build:
    for fname in os.listdir(build):
        if fname.endswith('.make'):
            path = os.path.join(build, fname)
            with open(path) as f:
                old = f.read()
            with open(path, 'w+') as f:
                f.write(re.sub(r'(CC|CXX)(\s*=\s*)(\S+)', compiler_repl, old))
else:
    print >> sys.stderr, 'post_premake.py --cc=gcc --cxx=g++ build'
