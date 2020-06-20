@echo off

cmd /c "scons p=windows bits=64 target=release_debug tools=yes -j12"

PAUSE