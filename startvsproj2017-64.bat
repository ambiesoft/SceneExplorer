@echo off
set DEL=sdel
set VCVARSBAT="C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars64.bat"
set DEVENV="C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\Common7\IDE\devenv.exe"

set QTVER=5.10.0
set QTTOOL=msvc2017_64

set SOURCEDIR=src
set PRONAME=SceneExplorer

funcvsproj.bat
