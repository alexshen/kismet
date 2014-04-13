#! /bin/bash

premake4 gmake && python change_gmake_compiler.py --cc=clang --cxx=clang++ build
