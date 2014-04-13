@echo off

pushd %~dp0

rem premake4 does not support vs2013, so generate a vs2012 solution and patch it.
premake4 vs2012
python switch_vs2012_to_vs2013.py build

popd
