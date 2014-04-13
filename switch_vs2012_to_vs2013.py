#! /usr/bin/python

import re
import glob
import sys
import os

build_dir = len(sys.argv) == 2 and sys.argv[1] or 'build'
for fname in glob.glob(os.path.join(build_dir, '*.vcxproj')):
    with open(fname) as f:
        data = f.read()
    with open(fname, 'w+') as f:
        f.write(re.sub(r'(<PlatformToolset>)(.*?)(</PlatformToolset>)', r'\1v120\3', data))
