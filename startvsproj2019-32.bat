@echo off
set DEL=sdel
set VCVARSBAT="C:\Program Files\Microsoft Visual Studio\2019\Community\Common7\Tools\VsDevCmd.bat"
set DEVENV="C:\Program Files\Microsoft Visual Studio\2019\Community\Common7\IDE\devenv.exe"

call %~dp0prepare.bat
set VSQTVER=%QTVERSION%
set VSQTTOOL=msvc2019


funcvsproj.bat
