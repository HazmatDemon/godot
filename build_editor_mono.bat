@echo off
scons p=windows target=release_debug tools=yes module_mono_enabled=yes -j6

PAUSE