@echo off
set DEL=sdel
set VCVARSBAT="C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
set DEVENV="C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\devenv.exe"

call %~dp0prepare.bat
set VSQTVER=%QTVERSION%
set VSQTTOOL=msvc2019_64


funcvsproj.bat
