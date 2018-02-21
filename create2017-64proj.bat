cd %~dp0
sdel SceneExplorer.sln
sdel SceneExplorer.vcxproj
sdel SceneExplorer.vcxproj.filters

set CANDIDATE1="C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars64.bat"
set CANDIDATE2="C:\Program Files\Microsoft Visual Studio\2017\Community\Common7\Tools\VsDevCmd.bat"

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

set QTBIN1=Y:\G\Qt\5.10.0\msvc2017_64\bin
set QTBIN2=C:\local\Qt\5.10.0\msvc2017_64\bin
set QMAKE=x
if exist %QTBIN1%\qmake.exe (
  set QTBIN=%QTBIN1%
  set QMAKE=%QTBIN1%\qmake.exe
) else (
if exist %QTBIN2%\qmake.exe (
  set QTBIN=%QTBIN2%
  set QMAKE=%QTBIN2%\qmake.exe
) )



if %QMAKE% == x (
  echo "qmake not found."
  goto :end
)

set PROFILE=src\SceneExplorer.pro
if not exist %PROFILE% (
  echo "pro not found."
  goto :end
)

call %VCVARSBAT%

cd %~dp0
echo %~dp0
call %QMAKE% -tp vc %PROFILE%

echo "==== Creating Visual Studio project successful ===="

set PATH=%QTBIN%;%PATH%
start "" "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\Common7\IDE\devenv.exe" SceneExplorer.vcxproj
exit

:end
pause
