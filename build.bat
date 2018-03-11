@echo off

if not exist %~dp0prepare.bat (
  echo prepare.bat not exist. Copy prepare.bat.sample and edit it.
  goto :error
)
call %~dp0prepare.bat

mkdir %DISTDIR%
if not exist %DISTDIR%\ (
  echo %DISTDIR% is not accessible.
  goto :error
)

copy /y %~dp0history.txt "%DISTDIR%\"
copy /y %~dp0README.md "%DISTDIR%\"
copy /y %~dp0LICENSE "%DISTDIR%\"

if not exist %PYTHONEXE% (
  echo %PYTHONEXE% not found. Check the directory.
  goto :error
)
if not exist %QTROOT% (
  echo %QTROOT% not found. Check the directory.
  goto :error
)

set QTPROJECTFILE=%SOURCEDIR%\%PRONAME%.pro
@echo on
%PYTHONEXE% ../distSolution/distqt.py %QTPROJECTFILE% -qtroot %QTROOT%
%FFCEXE% /t12 "%FFMPEGSOURCEDIR%" /to:%DISTDIR%\


@echo off

echo "Goint to close in 30 sec"
ping 127.0.0.1 -n 30 > nul
exit /b

:error
pause
exit /b
