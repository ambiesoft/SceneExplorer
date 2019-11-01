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

if %PYTHONEXE%x==x (
  echo PYTHONEXE not defined.
  goto :error
)
if not exist %PYTHONEXE% (
  echo PYTHONEXE %PYTHONEXE% not found. Check the directory.
  goto :error
)
if not exist %QTROOT%\ (
  echo QTROOT %QTROOT% not found. Check the directory.
  goto :error
)
if not exist %QTROOT%\%QTVERSION%\ (
  echo  QTROOT\QTVERSION %QTROOT%\%QTVERSION% not found. Check the directory.
  goto :error
)
if not exist %QTROOT%\%QTVERSION%\%QTVERSIONTOOLS%\ (
  echo %QTROOT%\%QTVERSION%\%QTVERSIONTOOLS% not found. Check the directory.
  goto :error
)
::if not exist %QTROOT%\Tools\%QTTOOLS%\ (
::  echo %QTROOT%\Tools\%QTTOOLS% not found. Check the directory.
::  goto :error
::)

if not exist %FFCEXE% (
  echo %FFCEXE% not found. Check the directory.
  goto :error
)
if %FFMPEGSOURCEDIR%x==x (
  echo FFMPEGSOURCEDIR not defined.
  goto :error
)
if not exist %FFMPEGSOURCEDIR%\ (
  echo %FFMPEGSOURCEDIR% not found. Check the directory.
  goto :error
)
if %FOLDERCONFIGDIR%x==x (
  echo FOLDERCONFIGDIR not defined.
  goto :error
)
if not exist %FOLDERCONFIGDIR%\ (
  echo %FOLDERCONFIGDIR% not found. Check the directory.
  goto :error
)

call :myCopy "%~dp0history.txt" "%DISTDIR%\"
call :myCopy "%~dp0README.md" "%DISTDIR%\"
call :myCopy "%~dp0README.jp.md" "%DISTDIR%\"
call :myCopy "%~dp0LICENSE" "%DISTDIR%\"
REM call :myXcopy "C:\Linkout\FolderConfig\*" "%DISTDIR%\" /E /Y
call :myCopy "%~dp0src\FolderConfig.ini" "%DISTDIR%\"


%FFCEXE% /t12 "%FFMPEGSOURCEDIR%" /to:%DISTDIR%\
%FFCEXE% /t12 "%FOLDERCONFIGDIR%\*" /to:%DISTDIR%\

set QTPROJECTFILE=%SOURCEDIR%\%PRONAME%.pro
@echo on
%PYTHONEXE% "%~dp0../distSolution/distqt.py" %QTPROJECTFILE% -distdir "%DISTDIR%" -qtroot %QTROOT% -qtversion %QTVERSION% -qtversiontools %QTVERSIONTOOLS% -distfile "%~dp0dist.json" -make %MAKE%
@echo off
if ERRORLEVEL 1 (
  goto :error
)


:success
::echo "Goint to close in 30 sec"
::ping 127.0.0.1 -n 30 > nul
::exit /b


:error
pause
exit /b

:myCopy
echo. copying %~1 to %~2
copy /y %~1 %~2
IF %ERRORLEVEL% NEQ 0 (
  echo. Failed copy %1
  pause
  call %~dp0ExitBatch.bat
)
exit /b

:myXcopy
echo. xcopying %~1 to %~2
xcopy %~1 %~2 /E /Y
IF %ERRORLEVEL% NEQ 0 (
  echo. Failed xcopy %1
  pause
  call %~dp0ExitBatch.bat
)
exit /b

