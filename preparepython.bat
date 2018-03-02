if not exist pythonexe.bat (
  echo pythonexe.bat not found. Rename pythonexe.bat.sample and edit it.
  goto :error
)
call pythonexe.bat
if not exist %PYTHONEXE% (
  echo %PYTHONEXE% not found. Check the directory.
  goto :error
)

exit /b

:error
pause
exit