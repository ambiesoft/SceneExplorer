@echo off
cd %~dp0
REM if not exist param.bat (
REM   echo Rename param.bat.sample to param.bat and edit it.
REM   goto :end
REM )
REM call param.bat

set SOLUTION=%PRONAME%\.sln
set VCXPROJ=%PRONAME%.vcxproj
set VCXFILTERS=%PRONAME%.vcxproj.filters
set PRO=%SOURCEDIR%\%PRONAME%.pro

sdel %SOLUTION%
sdel %VCXPROJ%
sdel %VCXFILTERS%

if not exist %VCVARSBAT% (
  echo %VCVARSBAT% not found.
  goto :end
)


set QMAKE=%QTROOT%\%QTVER%\%QTTOOL%\bin\qmake.exe

if not exist %QMAKE% (
  echo %QMAKE% not found.
  goto :end
)

if not exist %PRO% (
  echo %PRO% not found.
  goto :end
)

call %VCVARSBAT% %VCVARSBATARG%
cd %~dp0

call %QMAKE% -tp vc %PRO%
cd %~dp0

echo "==== Creating Visual Studio project successful ===="

set PATH=%QTBIN%;%PATH%
start "" %DEVENV% %VCXPROJ%
exit

:end
pause
