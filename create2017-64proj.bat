@echo off
cd %~dp0
if not exist param.bat (
  echo Rename param.bat.sample to param.bat and edit it.
  goto :end
)

call param.bat

sdel SceneExplorer.sln
sdel SceneExplorer.vcxproj
sdel SceneExplorer.vcxproj.filters

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

call %VCVARSBAT%
cd %~dp0

call %QMAKE% -tp vc %PRO%
cd %~dp0

echo "==== Creating Visual Studio project successful ===="

set PATH=%QTBIN%;%PATH%
start "" %DEVENV% SceneExplorer.vcxproj
exit

:end
pause
