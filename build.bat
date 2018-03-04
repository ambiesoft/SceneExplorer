@echo off
call prepareqt.bat
call preparepython.bat

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
