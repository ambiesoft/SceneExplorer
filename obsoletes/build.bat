REM sdel build
mkdir build
cd build

call "C:\Program Files\Microsoft Visual Studio 14.0\VC\bin\vcvars32.bat"
call C:\local\Qt\5.10.0\msvc2015\bin\qmake.exe ..\src\SceneExplorer.pro

call nmake
call C:\local\Qt\5.10.0\msvc2015\bin\windeployqt.exe --libdir C:\Linkout\SceneExplorer\ release\SceneExplorer.exe

mkdir C:\Linkout\SceneExplorer\platforms
copy C:\local\Qt\5.10.0\msvc2015\plugins\platforms\qwindows.dll C:\Linkout\SceneExplorer\platforms\
copy release\SceneExplorer.exe C:\Linkout\SceneExplorer\

pause



