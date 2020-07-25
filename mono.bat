@echo off

echo Select e(editor), etd(export template debug), etr(export template release)
set /p editor=

IF /i "%editor%"=="e" goto e
IF /i "%editor%"=="etd" goto etd
IF /i "%editor%"=="etr" goto etr

echo Wrong option
goto exit

:e
cmd /c "scons p=windows bits=64 tools=yes module_mono_enabled=yes mono_glue=no -j12"
cmd /c "bin\godot.windows.tools.64.mono.exe --generate-mono-glue modules/mono/glue"
cmd /c "scons p=windows bits=64 tools=yes target=release_debug module_mono_enabled=yes mono_glue=yes -j12"
:etd



:etr



:exit
PAUSE