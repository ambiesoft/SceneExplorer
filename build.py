import sys
import os
import timeit
import subprocess
from shutil import copyfile

QTVER='5.10.0'
TOOL='mingw53_32'

def myexit(message):
    print(message)
    exit(1)

def getQt():
    qtdirs = [
        "Y:\\local\\Qt",
        "C:\\local\\Qt",
    ]
    qtdir = ""
    for t in qtdirs:
        if os.path.isdir(t):
            qtdir=t
            break

    if not qtdir:
        myexit("Qt not found")

    return qtdir

def getQtToolBinDir():
    qtDirs = getQt()
    q = os.path.join(qtDirs,QTVER)
    if not os.path.isdir(q):
        myexit("{} is not found.".format(q))
    q = os.path.join(q, TOOL)
    if not os.path.isdir(q):
        myexit("{} is not found.".format(q))
    q = os.path.join(q, "bin")
    if not os.path.isdir(q):
        myexit("{} is not found.".format(q))

    return q

def getQmake():
    dir =getQtToolBinDir()

    qmake = os.path.join(dir, "qmake.exe")
    if not os.path.isfile(qmake):
        myexit("{} is not found.".format(qmake))

    return qmake

def getDeployTool():
    dir =getQtToolBinDir()

    qmake = os.path.join(dir, "windeployqt.exe")
    if not os.path.isfile(qmake):
        myexit("{} is not found.".format(qmake))

    return qmake

def getProfile():
    pro = "../src/SceneExplorer.pro"
    if not os.path.isfile(pro):
        myexit("profile {} not found.".format(pro))
    return pro

def getQtPluginPlatformDir():
    qtDirs = getQt()
    q = os.path.join(qtDirs,QTVER)
    if not os.path.isdir(q):
        myexit("{} is not found.".format(q))
    q = os.path.join(q, TOOL)
    if not os.path.isdir(q):
        myexit("{} is not found.".format(q))
    q = os.path.join(q, "plugins")
    if not os.path.isdir(q):
        myexit("{} is not found.".format(q))
    q = os.path.join(q, "platforms")
    if not os.path.isdir(q):
        myexit("{} is not found.".format(q))

    return q

def ensureDir(dir):
    if not os.path.isdir(dir):
        os.mkdir(dir)
        if not os.path.isdir(dir):
            myexit('Could not create {}'.format(dir))

def getBuildToolsBinDir():
    cand = [
        "C:\\local\\Qt\\Tools\\mingw530_32\\bin",
        "Y:\\local\\Qt\\Tools\\mingw530_32\\bin",
    ];
    ret = ""
    for t in cand:
        if os.path.isdir(t):
            ret=t
            break

    if not ret:
        myexit("Qt not found")

    return ret
    
def main():
    if not os.path.isdir("build"):
        os.mkdir("build")
        if not os.path.isdir("build"):
            myexit("Could not create dir [build]")

    toolbin = getBuildToolsBinDir()
    my_env = os.environ.copy()
    my_env["PATH"] = toolbin + os.pathsep + my_env["PATH"]
    os.environ['PATH']=my_env['PATH']
    print("{} is added to path.".format(toolbin))

    os.chdir("build")
    print("Entered directory {}".format(os.path.curdir))

    print("==== creating Makefile ====")
    qmake = getQmake()
    pro = getProfile()

    args = []
    args.append(qmake)
    args.append(pro)

    print(args)
    subprocess.check_call(args)


    print("==== make ====")
    make = "mingw32-make.exe"

    args=[]
    args.append(make)

    print(args)
    subprocess.check_call(args)


    print("==== deploying ====")
    args=[]
    deploytool = getDeployTool()
    releaseexe = "release/SceneExplorer.exe"
    if not os.path.isfile(releaseexe):
        myexit("Release exe {} not found.".format(releaseexe))

    distdir="C:\\Linkout\\SceneExplorer\\"
    ensureDir(distdir)

    args.append(deploytool)
    args.append(releaseexe)
    args.append('--libdir')
    args.append(distdir)
    print(args)
    subprocess.check_call(args)


    distplatformsdir = os.path.join(distdir, 'platforms')
    ensureDir(distplatformsdir)

    qwindowsdll = os.path.join(getQtPluginPlatformDir(),'qwindows.dll')
    if not os.path.isfile(qwindowsdll):
        myexit('qwindows.dll not found.')

    copyfile(qwindowsdll, os.path.join(distplatformsdir,'qwindows.dll'))
    copyfile(releaseexe, os.path.join(distdir,'SceneExplorer.exe'))


if __name__ == "__main__":
    # codetest()

    start = timeit.default_timer()
    main()
    stop = timeit.default_timer()

    m, s = divmod(stop - start, 60)
    h, m = divmod(m, 60)
    elapsed = "%d:%02d:%02d" % (h, m, s)

    print()
    print("==== Disribution Succeeded (elapsed: {}) ====".format(elapsed))

