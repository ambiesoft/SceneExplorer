@echo off
cd %~dp0
REM if not exist param.bat (
REM   echo Rename param.bat.sample to param.bat and edit it.
REM   goto :end
REM )
REM call param.bat

if not exist %~dp0\qtroot.bat (
  echo qtroot.bat not found. Rename qtroot.bat.sample and edit.
  goto end:
)
call %~dp0\qtroot.bat

if [%QTROOT%] == [] (
  echo QTROOT not defined.
  goto :end
)
if [%VCVARSBAT%] == [] (
  echo VCVARSBAT not defined.
  goto end:
)
if [%DEVENV%] == [] (
  echo DEVENV not defined.
  goto :end
)
if [%QTVER%] == [] (
  echo QTVER not defined.
  goto :end
)
if [%PRONAME%] == [] (
  echo PRONAME not defined.
  goto :end
)
if [%QTTOOL%] == [] (
  echo QTTOOL not defined.
  goto :end
)
if [%PRONAME%] == [] (
  echo PRONAME not defined.
  goto :end
)
if [%SOURCEDIR%] == [] (
  echo SOURCEDIR not defined.
  goto :end
)




set SOLUTION=%PRONAME%\.sln
set VCXPROJ=%PRONAME%.vcxproj
set VCXFILTERS=%PRONAME%.vcxproj.filters
set PRO=%SOURCEDIR%\%PRONAME%.pro

if not exist %PRO% (
  echo %PRO% not found.
  goto :end
)
sdel %SOLUTION%
sdel %VCXPROJ%
sdel %VCXFILTERS%

if not exist %VCVARSBAT% (
  echo %VCVARSBAT% not found.
  goto :end
)

set QTBIN=%QTROOT%\%QTVER%\%QTTOOL%\bin
set QMAKE=%QTBIN%\qmake.exe

if not exist %QMAKE% (
  echo %QMAKE% not found.
  goto :end
)

call %VCVARSBAT% %VCVARSBATARG%
cd %~dp0

call %QMAKE% -tp vc %PRO%
cd %~dp0

echo "==== Creating Visual Studio project successful ===="

set PATH=%QTBIN%;%PATH%
start "" %DEVENV% %VCXPROJ%
exit /b


:end
pause
