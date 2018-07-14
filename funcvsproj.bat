@echo off
cd %~dp0
REM if not exist param.bat (
REM   echo Rename param.bat.sample to param.bat and edit it.
REM   goto :end
REM )
REM call param.bat

if not exist %~dp0prepare.bat (
  echo prepare.bat not exist. Copy prepare.bat.sample and edit it.
  goto :end
)
call %~dp0prepare.bat

if [%DEL%] == [] (
  echo DEL not defined.
  goto :end
)
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
if [%VSQTVER%] == [] (
  echo VSQTVER not defined.
  goto :end
)
if [%PRONAME%] == [] (
  echo PRONAME not defined.
  goto :end
)
if [%VSQTTOOL%] == [] (
  echo VSQTTOOL not defined.
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

set QTBIN=%QTROOT%\%VSQTVER%\%VSQTTOOL%\bin
if not exist %QTBIN%\ (
  echo %QTBINT% not found or not a directory
  goto :end
)
set PATH=%QTBIN%;%PATH%



set SOLUTION=%PRONAME%.sln
set VCXPROJ=%PRONAME%.vcxproj
set VCXFILTERS=%PRONAME%.vcxproj.filters
set PRO=%SOURCEDIR%\%PRONAME%.pro

if not exist %PRO% (
  echo %PRO% not found.
  goto :end
)

if exist %SOLUTION% (
  echo "%SOLUTION% already exists, start launching it...
  goto :launchsln
)

%DEL% %SOLUTION%
if exist %SOLUTION% (
  echo Failed to delete %SOLUTION%.
  goto :end
)
%DEL% %VCXPROJ%
if exist %VCXPROJ% (
  echo Failed to delete %VCXPROJ%.
  goto :end
)
%DEL% %VCXFILTERS%
if exist %VCXFILTERS% (
  echo Failed to delete %VCXFILTERS%.
  goto :end
)

REM set OPENDBFILE=.vs\%PRONAME%\v15\Browse.VC.opendb
REM if exist %OPENDBFILE% (
REM   DEL %OPENDBFILE%
REM   if exist %OPENDBFILE% (
REM     echo %OPENDBFILE% exists. Close VS first.
REM     goto :end
REM   )
REM )

if not exist %VCVARSBAT% (
  echo %VCVARSBAT% not found.
  goto :end
)



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
echo launching %DEVENV% %VCXPROJ%
start "" %DEVENV% %VCXPROJ%
exit /b

:launchsln
start "" %DEVENV% %SOLUTION%
exit /b


:end
pause
