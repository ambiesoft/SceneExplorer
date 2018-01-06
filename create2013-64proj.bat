set CANDIDATE1="C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat"
set CANDIDATE2="C:\Program Files\Microsoft Visual Studio 12.0\VC\vcvarsall.bat"

set VCVARSBAT=x
if exist %CANDIDATE1% ( 
  set VCVARSBAT=%CANDIDATE1%
) else (
if exist %CANDIDATE2% (
  set VCVARSBAT=%CANDIDATE2%
) )

if %VCVARSBAT% == x (
  echo "vcvars not found."
  goto :end
)


set QMAKE=x
set QMAKECANDIDATE1=Y:\local\Qt\5.10.0\msvc2013_64\bin\qmake.exe
if exist %QMAKECANDIDATE1% (
  set QMAKE=%QMAKECANDIDATE1%
)

if %QMAKE% == x (
  echo "qmake not found."
  goto :end
)

set PROFILE=src/SceneExplorer.pro
if not exist %PROFILE% (
  echo "pro not found."
  goto :end
)

call %VCVARSBAT%
call %QMAKE% -tp vc %PROFILE%

echo "==== Creating Visual Studio project successful ===="

:end
pause
