@echo off

if not exist %~dp0prepare.bat (
  echo prepare.bat not exist. Rename prepare.bat.sample and edit it.
  goto :error
)
call %~dp0prepare.bat


if not exist %PYTHONEXE% (
  echo %PYTHONEXE% not found. Check the directory.
  goto :error
)
if not exist %QTROOT% (
  echo %QTROOT% not found. Check the directory.
  goto :error
)

@echo on
%PYTHONEXE% ../distSolution/distqt.py src/SceneExplorer.pro -qtroot %QTROOT%
@echo off


C:\LegacyPrograms\FFC\FFC.exe /t12 C:\LegacyPrograms\ffmpeg /to:C:\Linkout\SceneExplorer\


echo "Goint to close in 30 sec"
ping 127.0.0.1 -n 30 > nul
exit /b

:error
pause
exit /b
