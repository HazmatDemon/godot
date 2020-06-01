@echo off
scons p=windows tools=yes module_mono_enabled=yes mono_glue=no -j6

PAUSE