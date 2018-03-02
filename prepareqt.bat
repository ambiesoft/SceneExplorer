if not exist qtroot.bat (
  echo qtroot.bat not found. Rename qtroot.bat.sample and edit it.
  goto :error
)
call qtroot.bat
if not exist %QTROOT% (
  echo %QTROOT% not found. Check the directory.
  goto :error
)

exit /b

:error
pause
exit