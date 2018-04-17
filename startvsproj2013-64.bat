@echo off
set DEL=sdel
set VCVARSBAT="C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat"
set VCVARSBATARG=x64
set DEVENV="C:\Program Files (x86)\Microsoft Visual Studio 12.0\Common7\IDE\devenv.exe"


call %~dp0prepare.bat
set VSQTVER=%QTVERSION%
set VSQTTOOL=msvc2013_64


funcvsproj.bat
