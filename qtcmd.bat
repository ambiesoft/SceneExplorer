@echo off
call %~dp0prepareqt.bat
set PATH=%QTROOT%\5.10.0\mingw53_32\bin;%PATH%
set PATH=%QTROOT%\Tools\mingw530_32\bin;%PATH%

echo Qt is inserted in path.
@echo on
cmd /k
