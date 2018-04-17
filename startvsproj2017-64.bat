@echo off
set DEL=sdel
set VCVARSBAT="C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars64.bat"
set DEVENV="C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\Common7\IDE\devenv.exe"

call %~dp0prepare.bat
set VSQTVER=%QTVERSION%
set VSQTTOOL=msvc2017_64


funcvsproj.bat
