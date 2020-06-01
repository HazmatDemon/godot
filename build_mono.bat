@echo off
cmd /c generate_glue.bat
cmd /c generate_glue_sources.bat
cmd /c build_editor_mono.bat
set /p=
PAUSE