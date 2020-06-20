@echo off
cmd /c "scons p=windows bits=64 tools=no target=release -j12"
PAUSE