@echo off
cmd /c "scons p=windows bits=64 target=debug tools=no -j12"
PAUSE

