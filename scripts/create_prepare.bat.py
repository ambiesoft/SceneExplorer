import os
import re

template = """set PYTHONEXE=C:\\local\\python3\\python.exe
set QTROOT=C:\\local\\Qt
set QTVERSION={qtVer}

:: See under %QTROOT%\\%QTVERSION%
set QTVERSIONTOOLS={tool}

:: Uncomment if Visual Studio 2015
:: call "C:\\Program Files\\Microsoft Visual Studio 14.0\\Common7\\Tools\\VsDevCmd.bat"
:: set MAKE=nmake

:: Uncomment if Visual Studio 2017 x64
::call "C:\\Program Files (x86)\\Microsoft Visual Studio\\2017\\Community\\VC\\Auxiliary\\Build\\vcvars64.bat"
::set MAKE=nmake

:: Uncomment if Visual Studio 2019
::call "C:\\Program Files\\Microsoft Visual Studio\\2019\\Community\\Common7\\Tools\\VsDevCmd.bat"
::set MAKE=nmake

:: Uncomment if Visual Studio 2022
::call "C:\\Program Files\\Microsoft Visual Studio\\2022\\Community\\Common7\\Tools\\VsDevCmd.bat"
::set MAKE=nmake

:: Uncomment if MingW
::set PATH=%QTROOT%\\Tools\\mingw810_32\\bin;%PATH%
::set MAKE=mingw32-make.exe

:: Common setting
set SOURCEDIR=src
set PRONAME=SceneExplorer

:: Output directory
set DISTDIR=C:\\Linkout\\SceneExplorer

:: Files under this directory will be copied to Output directory,
:: must contain bin/ffprobe.exe and bin/ffmpeg.exe
set FFMPEGSOURCEDIR=C:\\LegacyPrograms\\ffmpeg


:: Support tools
:: https://github.com/ambiesoft/FolderConfig/releases
set FOLDERCONFIGDIR=C:\\Linkout\\FolderConfig

:: https://www.vector.co.jp/soft/winnt/util/se378224.html
set FFCEXE=C:\\LegacyPrograms\\FFC\\FFC.exe
"""


def main():
    # qtDir = input("Enter QT install directory:")
    qtDir = "C:\\local\\Qt"
    output = ""

    qtVers = []
    dirs = [d for d in os.listdir(
        qtDir) if os.path.isdir(os.path.join(qtDir, d))]

    # find Qt version from QT installed directory
    for dir in dirs:
        if re.match(r'^[\d\.]+$', dir):
            qtVers.append(dir)
    if not qtVers:
        exit("No Qt versions found")

    if len(qtVers) > 1:
        question = "Select Qt version\n"
        i = 1
        for qtVer in qtVers:
            question += str(i)
            question += ":"
            question += qtVer
            question += "\n"
        i = int(input(question + ">"))
        if not i:
            exit("No Qt version selected")
        qtVer = qtVers[i-1]
    else:
        qtVer = qtVers[0]

    if not qtVer:
        exit("No Qt version")

    # find tools
    qtVerDir = os.path.join(qtDir, qtVer)
    dirs = [d for d in os.listdir(
        qtVerDir) if os.path.isdir(os.path.join(qtVerDir, d))]
    tools = []
    for dir in dirs:
        tools.append(dir)
    if not tools:
        exit("No tools found")
    if len(tools) > 1:
        question = "Select Qt tool\n"
        i = 1
        for tool in tools:
            question += str(i)
            question += ":"
            question += tool
            question += "\n"
        i = int(input(question + ">"))
        if not i:
            exit("No tool selected")
        tool = tools[i-1]
    else:
        tool = tools[0]

    if not tool:
        exit("No tool")

    print(template.format(qtVer=qtVer, tool=tool))


if __name__ == "__main__":
    main()
