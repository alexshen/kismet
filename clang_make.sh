#! /bin/bash

premake4 gmake && python post_premake.py --cc=clang --cxx=clang++ build
