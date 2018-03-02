call qtroot.bat

C:\local\python3.5\python.exe ../distSolution/distqt.py src/SceneExplorer.pro -qtroot %QTROOT%

echo "Goint to closing in 30 sec"
ping 127.0.0.1 -n 30 > nul