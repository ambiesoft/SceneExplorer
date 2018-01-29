REM ==========================================================
set CANDIDATE1="C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat"
set CANDIDATE2="C:\Program Files\Microsoft Visual Studio 12.0\VC\vcvarsall.bat"

set QTBIN1=C:\local\Qt\5.10.0\msvc2013_64\bin
set QTBIN2=Y:\G\Qt\5.10.0\msvc2013_64\bin

set PROFILE=src/SceneExplorer.pro

REM ==========================================================


cd %~dp0
sdel SceneExplorer.sln
sdel SceneExplorer.vcxproj
sdel SceneExplorer.vcxproj.filters

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


if exist %QTBIN1% (
  set QTBIN=%QTBIN1%
) else (
if exist %QTBIN2% (
  set QTBIN=%QTBIN2%
) )

set QMAKE=x
set QMAKECANDIDATE1=%QTBIN%\qmake.exe
if exist %QMAKECANDIDATE1% (
  set QMAKE=%QMAKECANDIDATE1%
)

if %QMAKE% == x (
  echo "qmake not found."
  goto :end
)


if not exist %PROFILE% (
  echo "pro not found."
  goto :end
)

call %VCVARSBAT%
call %QMAKE% -tp vc %PROFILE%

echo "==== Creating Visual Studio project successful ===="

set PATH=%QTBIN%;%PATH%
start "" "C:\Program Files (x86)\Microsoft Visual Studio 12.0\Common7\IDE\devenv.exe" SceneExplorer.vcxproj
exit

:end
pause
