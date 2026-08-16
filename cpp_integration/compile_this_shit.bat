@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat"
scons platform=windows debug_symbols=yes dev_build=yes
pause