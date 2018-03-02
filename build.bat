@echo off
if not exist qtroot.bat (
  echo qtroot.bat not found. Rename qtroot.bat.sample and edit it.
  goto :error
)
call qtroot.bat
if not exist %QTROOT% (
  echo %QTROOT% not found. Check the directory.
  goto :error
)
C:\local\python3.5\python.exe ../distSolution/distqt.py src/SceneExplorer.pro -qtroot %QTROOT%

echo "Goint to close in 30 sec"
ping 127.0.0.1 -n 30 > nul
exit /b

:error
pause
exit /b
