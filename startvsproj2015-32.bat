@echo off
set DEL=sdel
set VCVARSBAT="C:\Program Files\Microsoft Visual Studio 14.0\VC\vcvarsall.bat"
set DEVENV="C:\Program Files\Microsoft Visual Studio 14.0\Common7\IDE\devenv.exe"

call %~dp0prepare.bat
set VSQTVER=%QTVERSION%
set VSQTTOOL=msvc2015


funcvsproj.bat
