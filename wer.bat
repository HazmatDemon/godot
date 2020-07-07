@echo off

scons p=windows tools=yes bits=64 target=release_debug -j4 
PAUSE